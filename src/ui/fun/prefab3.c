// 2D and 3D uis spawn through the same functions (spawn_ui, spawn_text, spawn_window): under a 3D parent (a canvas3, or
//  a 3D element) a 2D prefab is swapped for its 3D twin (Prefab3: made by the same prefab function on a 3D base,
//  windows/pre/twins.c), a layer above its parent's (a canvas3 draws its elements in layer order)
//  - spawned in a system the new entities' components aren't there yet (deferred), so the 3D ones spawned this frame
//    are remembered (ui3_spawned): their children, spawned the same frame, find them 3D
#define ui3_spawned_max 1024
static entity ui3_spawned[ui3_spawned_max];
static double ui3_spawned_time[ui3_spawned_max];
static int ui3_spawned_next = 0;
static double ui3_spawned_last = -1;     // when one was last remembered: none this frame, no search (2D's spawns)

static inline void ui3_spawned_add(entity e) {
    ui3_spawned_last = zox_current_time;
    ui3_spawned[ui3_spawned_next] = e;
    ui3_spawned_time[ui3_spawned_next] = zox_current_time;
    ui3_spawned_next = (ui3_spawned_next + 1) % ui3_spawned_max;
}

static inline byte ui3_spawned_has(entity e) {
    if (ui3_spawned_last != zox_current_time) {
        return 0;
    }
    for (int i = 0; i < ui3_spawned_max; i++) {
        if (ui3_spawned[i] == e && ui3_spawned_time[i] == zox_current_time) {
            return 1;
        }
    }
    return 0;
}

static inline byte ui_is3D(ecs* world, entity e) {
    return zox_valid(e) && (zox_has(e, Canvas3) || zox_has(e, LocalPosition3D) || ui3_spawned_has(e));
}

static inline entity ui_prefab_for(ecs* world, entity prefab, entity parent) {
    if (ui_is3D(world, parent) && zox_has(prefab, Prefab3)) {
        const entity twin = zox_getv(prefab, Prefab3);
        if (zox_valid(twin)) {
            return twin;
        }
    }
    return prefab;
}

// a 3D element's layer, above its parent's
static byte ui3_spawned_layer[ui3_spawned_max];

static inline byte ui3_layer_of(ecs* world, entity e) {
    for (int i = 0; i < ui3_spawned_max && ui3_spawned_last == zox_current_time; i++) {
        if (ui3_spawned[i] == e && ui3_spawned_time[i] == zox_current_time) {
            return ui3_spawned_layer[i];
        }
    }
    return zox_has(e, Layer) && !zox_has(e, Canvas3) ? zox_getv(e, Layer) : 0;
}

// a 3D element (its parent 3D): a layer above its parent's, and remembered for its own children
static inline void ui_layer3_set(ecs* world, entity e, entity parent) {
    if (ui_is3D(world, parent)) {
        const byte layer = ui3_layer_of(world, parent) + 1;
        zox_setv(e, Layer, layer);
        ui3_spawned_layer[ui3_spawned_next] = layer;
        ui3_spawned_add(e);
    }
}

// a 3D root (a canvas3 window), its children 3D
static inline void ui3_root_spawned(entity e) {
    ui3_spawned_layer[ui3_spawned_next] = 0;
    ui3_spawned_add(e);
}
