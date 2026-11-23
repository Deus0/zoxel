// blocks whose model is a blueprint (a model node graph, doc/tech/nodes.md), seeded by their realm: each realm's
//  its own (a gravity cube now; the base blocks later)

// a colors node: count shades of a colour seeded in a hue, saturation and value range (0 to 360, 100, 100)
entity spawn_node_model_colors_hsv(ecs* world, byte count, float6 hsv) {
    entity e = spawn_node_model_colors_seed(world, count);
    zox_setv(e, ColorHSV, hsv);
    return e;
}

// a traces node: count circuit traces of voxel over the model's surface (eve/traces.c)
entity spawn_node_model_traces(ecs* world, byte voxel, byte count) {
    entity e = spawn_node_model(world, prefab_node_model, zox_model_node_traces);
    zox_name("node_model_traces");
    zox_setv(e, NodeVoxel, voxel);
    zox_setv(e, Count, count);
    return e;
}

// a box of voxel from a corner, in blueprint voxels (voxel 0 cuts)
static inline entity spawn_node_model_box(ecs* world, byte3 position, byte3 size, byte voxel) {
    return spawn_node_model_at(world, prefab_node_model, zox_model_node_box, position, size, voxel);
}

// links b after a in a chain, returns b
static inline entity node_chain(ecs* world, entity a, entity b) {
    new_link_single_node(world, a, b);
    return b;
}

// a gravity cube: cyberpunk tech, a dark metal box in panels, neon edges, a glowing core through a window on each
//  face, neon circuit traces and lights; its colours (hues, shades) and traces seeded, so each realm's differs
//  - voxels: 1 metal, 2 seams, 3 neon, 4 traces (neon's second shade), 5 the core
entity spawn_model_nodegraph_gravity_cube(ecs* world, entity parent) {
    const byte3 size = get_scaled_size(nodegraph_max_depth, float3_one);
    const byte l = size.x;      // 32
    const byte t = l / 16;      // edges 2 thick
    entity root = spawn_node_model_size(world, float3_one, float3_one);
    entity n = root;
    n = node_chain(world, n, spawn_node_model_colors_hsv(world, 2, (float6) { 0, 360, 5, 22, 12, 24 }));
    n = node_chain(world, n, spawn_node_model_colors_hsv(world, 2, (float6) { 0, 360, 85, 100, 88, 100 }));
    n = node_chain(world, n, spawn_node_model_colors_hsv(world, 1, (float6) { 0, 360, 60, 95, 95, 100 }));
    n = node_chain(world, n, spawn_node_model_box(world, byte3_zero, size, 1));
    // panel seams: slabs through it at a third and two thirds, lines across every face
    for (byte axis = 0; axis < 3; axis++) {
        for (byte k = 1; k <= 2; k++) {
            byte3 p = byte3_zero, s = size;
            byte* pa = axis == 0 ? &p.x : (axis == 1 ? &p.y : &p.z);
            byte* sa = axis == 0 ? &s.x : (axis == 1 ? &s.y : &s.z);
            *pa = l * k / 3;
            *sa = 1;
            n = node_chain(world, n, spawn_node_model_box(world, p, s, 2));
        }
    }
    // neon edges: the 12, t thick
    for (byte axis = 0; axis < 3; axis++) {
        for (byte corner = 0; corner < 4; corner++) {
            const byte a = corner & 1 ? l - t : 0, b = corner & 2 ? l - t : 0;
            const byte3 p = axis == 0 ? (byte3) { 0, a, b } : (axis == 1 ? (byte3) { a, 0, b } : (byte3) { a, b, 0 });
            const byte3 s = axis == 0 ? (byte3) { l, t, t } : (axis == 1 ? (byte3) { t, l, t } : (byte3) { t, t, l });
            n = node_chain(world, n, spawn_node_model_box(world, p, s, 3));
        }
    }
    // the core, then a window into it on each face in a neon frame (sizes in eighths of the box, so they halve
    //  exactly down to a 16 model: an odd size lost the outer layer, the window didn't show on its + faces)
    const byte center = l / 2, core = l * 5 / 8, window = l * 3 / 8, deep = l / 4;
    const byte w0 = center - window / 2;
    n = node_chain(world, n, spawn_node_model_at(world, prefab_node_model, zox_model_node_fill, byte3_single(center),
        byte3_single(core), 5));
    for (byte face = 0; face < 6; face++) {
        for (int frame = 1; frame >= 0; frame--) {   // its frame, then the window cut through it
            const byte axis = face / 2;
            const byte w = frame ? window + 2 * t : window, start = frame ? w0 - t : w0, depth = frame ? t : deep;
            const byte d = face % 2 ? l - depth : 0;
            const byte3 p = axis == 0 ? (byte3) { d, start, start } : (axis == 1 ? (byte3) { start, d, start } : (byte3) { start, start, d });
            const byte3 s = axis == 0 ? (byte3) { depth, w, w } : (axis == 1 ? (byte3) { w, depth, w } : (byte3) { w, w, depth });
            n = node_chain(world, n, spawn_node_model_box(world, p, s, frame ? 3 : 0));
        }
    }
    n = node_chain(world, n, spawn_node_model_traces(world, 4, 24));
    n = node_chain(world, n, spawn_node_model_sprinkle(world, zox_model_node_speckle, 3, 8, 0, byte3_zero, size));
    node_chain(world, n, spawn_node_model_end(world));
    zox_set_parent(world, root, parent);
    return root;
}

// a realm's solid block whose model is a blueprint, its faces baked from it (as a stone's from its model), the
//  model one variant at block_depth seeded by seed
entity spawn_realm_block_blueprint(ecs* world, entity realm, entity nodegraph, lint seed, const char* name, color primary,
    byte dbg_log)
{
    char model_name[64];
    snprintf(model_name, sizeof(model_name), "%s_model", name);
    const entity2 models = spawn_blueprint_models(world, realm, nodegraph, seed, model_name, block_depth, 1);
    return spawn_realm_block_baked_model(world, prefab_block_baked, realm, seed, name, primary, models.y, 1, dbg_log);
}
