// tst: flecs_event_costs, what each way of reacting to a change costs a frame, to choose between them (doc/tech/states.md)
//  - each case in its own small world (ecs_init, single threaded), [zox_tst_ec_frames] frames timed after a warm up,
//      so the game's frame (rendering, streaming) doesn't drown it out
//  - idle: [zox_tst_ec_count] systems matching nothing / observers never fired / byte states (an increment system
//      each, on [zox_tst_ec_entities] entities at 0), what they cost while nothing happens
//  - an event a frame (a tag added to one entity) handled by: a system that removes it, an observer that removes it,
//      a pair (TDirty to TUpdate by 2 systems, a reader on TUpdate)
//  - logs each per frame (us) and per system or observer; checks every event was handled (once per frame each)
//  - run: bash bsh/tst.sh flecs_event_costs
#define zox_tst_ec_frames 4000
#define zox_tst_ec_count 50
#define zox_tst_ec_entities 1000
ecs_entity_t zox_tst_ec_tags[zox_tst_ec_count];
ecs_entity_t zox_tst_ec_dirty = 0;
ecs_entity_t zox_tst_ec_update = 0;
int zox_tst_ec_handled = 0;
double zox_tst_ec_us[8];
int zox_tst_ec_events[8];

static void zox_tst_ec_noop(ecs_iter_t* it) {
    (void) it;
}

// the event's reader: counts it, removes the tag (a system or an observer)
static void zox_tst_ec_handle(ecs_iter_t* it) {
    for (int i = 0; i < it->count; i++) {
        ecs_remove_id(it->stage, it->entities[i], zox_tst_ec_tags[0]);
        zox_tst_ec_handled++;
    }
}

// a pair's reader on TUpdate, and its two removers (as zoxc_system_dirty)
static void zox_tst_ec_pair_read(ecs_iter_t* it) {
    zox_tst_ec_handled += it->count;
}

static void zox_tst_ec_pair_dirty(ecs_iter_t* it) {
    for (int i = 0; i < it->count; i++) {
        ecs_remove_id(it->stage, it->entities[i], zox_tst_ec_dirty);
        ecs_add_id(it->stage, it->entities[i], zox_tst_ec_update);
    }
}

static void zox_tst_ec_pair_update(ecs_iter_t* it) {
    for (int i = 0; i < it->count; i++) {
        ecs_remove_id(it->stage, it->entities[i], zox_tst_ec_update);
    }
}

// a byte state's increment system (zoxc_state): trigger, active, end, back to 0
static void zox_tst_ec_increment(ecs_iter_t* it) {
    byte* values = ecs_field_w_size(it, sizeof(byte), 0);
    for (int i = 0; i < it->count; i++) {
        if (values[i]) {
            values[i] = values[i] < 3 ? values[i] + 1 : 0;
        }
    }
}

static ecs_entity_t zox_tst_ec_system(ecs_world_t* w, ecs_id_t id, ecs_iter_action_t callback, ecs_id_t without) {
    ecs_system_desc_t desc = { 0 };
    desc.entity = ecs_new(w);
    ecs_add_pair(w, desc.entity, EcsDependsOn, EcsOnUpdate);
    desc.query.terms[0] = (ecs_term_t) { .id = id };
    if (without) {
        desc.query.terms[1] = (ecs_term_t) { .id = without, .oper = EcsNot };
    }
    desc.callback = callback;
    return ecs_system_init(w, &desc);
}

static void zox_tst_ec_observer(ecs_world_t* w, ecs_id_t id, ecs_iter_action_t callback) {
    ecs_observer_desc_t desc = { 0 };
    desc.query.terms[0] = (ecs_term_t) { .id = id };
    desc.events[0] = EcsOnAdd;
    desc.callback = callback;
    ecs_observer_init(w, &desc);
}

// times a world's frames, the event (tag added to one entity) each frame if [fire]: us a frame
static double zox_tst_ec_time(ecs_world_t* w, ecs_entity_t target, ecs_id_t fire) {
    for (int f = 0; f < 50; f++) {
        if (fire) {
            ecs_add_id(w, target, fire);
        }
        ecs_progress(w, 0);
    }
    zox_tst_ec_handled = 0;
    const double start = zox_observer_now_ms();
    for (int f = 0; f < zox_tst_ec_frames; f++) {
        if (fire) {
            ecs_add_id(w, target, fire);
        }
        ecs_progress(w, 0);
    }
    return (zox_observer_now_ms() - start) * 1000.0 / zox_tst_ec_frames;
}

static ecs_world_t* zox_tst_ec_world(ecs_entity_t* target) {
    ecs_world_t* w = ecs_init();
    for (int i = 0; i < zox_tst_ec_count; i++) {
        zox_tst_ec_tags[i] = ecs_new(w);
    }
    zox_tst_ec_dirty = ecs_new(w);
    zox_tst_ec_update = ecs_new(w);
    *target = ecs_new(w);
    return w;
}

static void zox_tst_ec_case(int index, const char* name) {
    ecs_entity_t target = 0;
    ecs_world_t* w = zox_tst_ec_world(&target);
    ecs_id_t fire = 0;
    if (index == 1) {           // idle systems
        for (int i = 0; i < zox_tst_ec_count; i++) {
            zox_tst_ec_system(w, zox_tst_ec_tags[i], zox_tst_ec_noop, 0);
        }
    } else if (index == 2) {    // idle observers
        for (int i = 0; i < zox_tst_ec_count; i++) {
            zox_tst_ec_observer(w, zox_tst_ec_tags[i], zox_tst_ec_noop);
        }
    } else if (index == 3) {    // idle byte states, an increment system each
        for (int i = 0; i < zox_tst_ec_count; i++) {
            ecs_component_desc_t desc = { 0 };
            desc.entity = ecs_new(w);
            desc.type.size = sizeof(byte);
            desc.type.alignment = sizeof(byte);
            const ecs_entity_t component = ecs_component_init(w, &desc);
            for (int e = 0; e < zox_tst_ec_entities / zox_tst_ec_count; e++) {
                const ecs_entity_t entity = ecs_new(w);
                const byte zero = 0;
                ecs_set_id(w, entity, component, sizeof(byte), &zero);
            }
            zox_tst_ec_system(w, component, zox_tst_ec_increment, 0);
        }
    } else if (index == 4) {    // an event a frame, a system removes it
        zox_tst_ec_system(w, zox_tst_ec_tags[0], zox_tst_ec_handle, 0);
        fire = zox_tst_ec_tags[0];
    } else if (index == 5) {    // an event a frame, an observer removes it
        zox_tst_ec_observer(w, zox_tst_ec_tags[0], zox_tst_ec_handle);
        fire = zox_tst_ec_tags[0];
    } else if (index == 6) {    // an event a frame, a pair: reader on TUpdate, TDirty to TUpdate, TUpdate removed
        zox_tst_ec_system(w, zox_tst_ec_update, zox_tst_ec_pair_read, 0);
        zox_tst_ec_system(w, zox_tst_ec_dirty, zox_tst_ec_pair_dirty, 0);
        zox_tst_ec_system(w, zox_tst_ec_update, zox_tst_ec_pair_update, zox_tst_ec_dirty);
        fire = zox_tst_ec_dirty;
    }
    zox_tst_ec_us[index] = zox_tst_ec_time(w, target, fire);
    zox_tst_ec_events[index] = zox_tst_ec_handled;
    ecs_fini(w);
    zox_log("[tst] [metric] flecs_event_costs %s: [%.2fus] a frame", name, zox_tst_ec_us[index]);
}

byte zox_tst_event_costs_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_event_costs_check(ecs* world) {
    (void) world;
    zox_tst_ec_case(0, "an empty world");
    zox_tst_ec_case(1, "50 idle systems");
    zox_tst_ec_case(2, "50 idle observers");
    zox_tst_ec_case(3, "50 idle byte states (1000 entities)");
    zox_tst_ec_case(4, "an event a frame, a system");
    zox_tst_ec_case(5, "an event a frame, an observer");
    zox_tst_ec_case(6, "an event a frame, a pair");
    const double base = zox_tst_ec_us[0];
    zox_log("[tst] [metric] flecs_event_costs idle, each: a system [%.3fus], an observer [%.3fus], a byte state [%.3fus]",
        (zox_tst_ec_us[1] - base) / zox_tst_ec_count, (zox_tst_ec_us[2] - base) / zox_tst_ec_count,
        (zox_tst_ec_us[3] - base) / zox_tst_ec_count);
    zox_log("[tst] [metric] flecs_event_costs an event, over the empty world: a system [%.2fus], an observer [%.2fus], a pair [%.2fus]",
        zox_tst_ec_us[4] - base, zox_tst_ec_us[5] - base, zox_tst_ec_us[6] - base);
    zox_tst_check(zox_tst_ec_events[4] == zox_tst_ec_frames, "the system handled [%i] of [%i] events",
        zox_tst_ec_events[4], zox_tst_ec_frames);
    zox_tst_check(zox_tst_ec_events[5] == zox_tst_ec_frames, "the observer handled [%i] of [%i] events",
        zox_tst_ec_events[5], zox_tst_ec_frames);
    // a pair's tag is on for 2 frames (TDirty, then TUpdate), added again each frame: its reader sees it every frame
    zox_tst_check(zox_tst_ec_events[6] >= zox_tst_ec_frames - 1, "the pair's reader saw [%i] of [%i] events",
        zox_tst_ec_events[6], zox_tst_ec_frames);
}
