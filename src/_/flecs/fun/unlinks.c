/*
 * Unlink Callbacks
 * ================
 *
 * Calls a function when a link is removed, for any reason:
 *  - zox_unlink / zox_set_link(.., 0)
 *  - replaced by a new target (single links), with the old target
 *  - the linking entity is deleted
 *  - the target is deleted, including by cascade (i.e. its parent was deleted)
 *
 *     void on_unlink_ChunkLink(ecs* world, entity e, entity chunk) {
 *         if (!zox_valid(chunk)) {
 *             return;
 *         }
 *         ...
 *     }
 *
 *     zoxd_nf_link(ChunkLink);
 *     zox_on_unlink(on_unlink_ChunkLink, ChunkLink);
 *
 * Deletion code doesn't need to know about links, cleanup is handled here.
 *
 * NOTE: Callbacks are queued, and called once per frame at zoxp_begin, outside cleanup,
 *  so they can add, remove and delete as normal
 * NOTE: e may be deleted by then (when the linking entity was deleted), check zox_valid
 * NOTE: target is 0 if it was deleted
 *
 * How: flecs doesn't emit OnRemove to observers when a non fragmenting pair is removed
 *  because its target was deleted, but it does call the relation's on_remove hook.
 *  Hooks need a type, so the link becomes a 1 byte component. Hooks can't make
 *  structural changes during cleanup, so they queue the callback instead.
 */

typedef void (*zox_unlink_fun)(ecs* world, entity e, entity target);

typedef struct {
    zox_unlink_fun callback;
    entity e;
    entity target;
} zox_unlink_event;

zox_unlink_event* zox_unlinks = NULL;
uint zox_unlinks_count = 0;
uint zox_unlinks_capacity = 0;

// NOTE: Hooks only run when commands merge, on the main thread
static void zox_unlink_hook(iter* it) {
    zox_unlink_fun callback = (zox_unlink_fun) it->ctx;
    // target id may be stale if deleted, so get its alive version or 0
    entity target = ecs_get_alive(it->world, ECS_PAIR_SECOND(it->ids[0]));
    if (zox_unlinks_count + it->count > zox_unlinks_capacity) {
        uint capacity = zox_unlinks_capacity ? zox_unlinks_capacity * 2 : 64;
        while (capacity < zox_unlinks_count + it->count) {
            capacity *= 2;
        }
        zox_unlink_event* resized = realloc(zox_unlinks, capacity * sizeof(zox_unlink_event));
        if (!resized) {
            zox_loge("[zox_unlink_hook] failed to grow queue to [%i]", capacity);
            return;
        }
        zox_unlinks = resized;
        zox_unlinks_capacity = capacity;
    }
    for (int i = 0; i < it->count; i++) {
        zox_unlinks[zox_unlinks_count++] = (zox_unlink_event) {
            .callback = callback,
            .e = it->entities[i],
            .target = target
        };
    }
}

// Gives a link relation a 1 byte type, so it can have an on_remove hook
void zox_on_unlink_internal(
    ecs* world,
    entity relation,
    zox_unlink_fun callback)
{
    ecs_component_init(world, &(ecs_component_desc_t) {
        .entity = relation,
        .type = {
            .size = 1,
            .alignment = 1
        }
    });
    ecs_set_hooks_id(world, relation, &(ecs_type_hooks_t) {
        .on_remove = zox_unlink_hook,
        .ctx = (void*) callback
    });
}

#define zox_on_unlink(callback, T) \
    zox_on_unlink_internal(world, zox_id(T), callback)

// Calls queued unlink callbacks, events queued during these run next frame
void UnlinksSystem(iter* it) {
    if (!zox_unlinks_count) {
        return;
    }
    ecs* world = it->world;
    const uint count = zox_unlinks_count;
    zox_unlink_event* events = zox_unlinks;
    // swap out so callbacks can queue new events safely
    zox_unlinks = NULL;
    zox_unlinks_count = 0;
    zox_unlinks_capacity = 0;
    for (uint i = 0; i < count; i++) {
        // NOTE: a target deleted during cleanup was still alive when queued, it's 0 now (tst links_api)
        entity target = events[i].target;
        if (target && !ecs_is_alive(world, target)) {
            target = 0;
        }
        events[i].callback(world, events[i].e, target);
    }
    free(events);
} zoxd_system(UnlinksSystem);

void zox_unlinks_initialize(ecs* world) {
    zox_system_1(UnlinksSystem, zoxp_begin);
}
