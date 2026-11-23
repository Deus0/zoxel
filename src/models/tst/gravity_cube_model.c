// tst: gravity_cube_model, the gravity cube's blueprint (models/ins/block_blueprint.c), at a block's depth
//  - three models of it: seed A, seed A again, seed B
//  - the same seed makes the same cube (voxels and colours), another seed other colours
//  - each: its 5 colours, the metal dark (value under 30) and the neon bright and saturated (over 80); its 8 corners
//    neon (its edges, or a trace over them); a window on each of its 6 faces (air at the face's middle, through its
//    outer layer: an odd size lost it on the + faces once) onto its core or a trace on it
//  - to look at it: zox_tst_model=gravity zox_tst_seed=<seed> bash bsh/tst.sh model_views
//  - run: bash bsh/tst.sh gravity_cube_model
entity zox_tst_gcm_models[3];
int zox_tst_gcm_frames = 0;

byte zox_tst_gravity_cube_model_ready(ecs* world) {
    if (!zox_tst_gcm_models[0]) {
        const entity holder = zox_new();
        const entity graph = spawn_model_nodegraph_gravity_cube(world, holder);
        const lint seeds[3] = { 666, 666, 4242 };
        const char* names[3] = { "tst_cube_a", "tst_cube_a2", "tst_cube_b" };
        for (byte i = 0; i < 3; i++) {
            zox_tst_gcm_models[i] = spawn_blueprint_models(world, holder, graph, seeds[i], names[i], block_depth, 1).y;
        }
        return 0;
    }
    zox_tst_waiting = "the gravity cube models to generate";
    zox_tst_gcm_frames++;
    if (zox_tst_gcm_frames < 3) {
        return 0;
    }
    for (byte i = 0; i < 3; i++) {
        if (zox_has(zox_tst_gcm_models[i], Generating)) {
            return 0;
        }
    }
    return 1;
}

static inline byte zox_tst_gcm_voxel(ecs* world, entity model, int x, int y, int z) {
    return getv_VoxelNode(zox_get(model, VoxelNode), zox_getv(model, NodeDepth), (byte3) { x, y, z });
}

static inline int zox_tst_gcm_differ(ecs* world, entity a, entity b) {
    const int length = octree_size(zox_getv(a, NodeDepth));
    int differ = 0;
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        differ += zox_tst_gcm_voxel(world, a, x, y, z) != zox_tst_gcm_voxel(world, b, x, y, z);
    }
    return differ;
}

static inline byte zox_tst_gcm_colors_equal(ecs* world, entity a, entity b) {
    const ColorRGBs* ca = zox_get(a, ColorRGBs);
    const ColorRGBs* cb = zox_get(b, ColorRGBs);
    if (ca->length != cb->length) {
        return 0;
    }
    for (int i = 0; i < ca->length; i++) {
        if (ca->value[i].r != cb->value[i].r || ca->value[i].g != cb->value[i].g || ca->value[i].b != cb->value[i].b) {
            return 0;
        }
    }
    return 1;
}

static inline void zox_tst_gcm_check_cube(ecs* world, entity model, const char* label) {
    const int length = octree_size(zox_getv(model, NodeDepth));
    const ColorRGBs* colors = zox_get(model, ColorRGBs);
    zox_tst_check(colors->length == 5, "[%s] has [%i] colours, not metal 2, neon 2 and its core", label, colors->length);
    if (colors->length >= 3) {
        const float3 metal = color_rgb_to_hsv(colors->value[0]);
        const float3 neon = color_rgb_to_hsv(colors->value[2]);
        zox_tst_check(metal.z < 30, "[%s] its metal isn't dark, value [%.0f]", label, metal.z);
        zox_tst_check(neon.y > 80 && neon.z > 80, "[%s] its neon isn't bright, saturation [%.0f] value [%.0f]", label, neon.y, neon.z);
    }
    int corners = 0;
    for (int c = 0; c < 8; c++) {
        const byte voxel = zox_tst_gcm_voxel(world, model, c & 1 ? length - 1 : 0, c & 2 ? length - 1 : 0, c & 4 ? length - 1 : 0);
        corners += voxel == 3 || voxel == 4;
    }
    zox_tst_check(corners == 8, "[%s] only [%i] of its 8 corners are neon", label, corners);
    int windows = 0;
    const int m = length / 2;
    for (byte face = 0; face < 6; face++) {
        const byte axis = face / 2;
        const int sign = face % 2 ? -1 : 1;     // inward
        int3 p = { m, m, m };
        *int3_axis(&p, axis) = face % 2 ? length - 1 : 0;
        const byte outer_air = !zox_tst_gcm_voxel(world, model, p.x, p.y, p.z);
        byte inside = 0;
        for (int d = 0; d < length && !inside; d++) {
            inside = zox_tst_gcm_voxel(world, model, p.x, p.y, p.z);
            *int3_axis(&p, axis) += sign;
        }
        windows += outer_air && (inside == 5 || inside == 4);
    }
    zox_tst_check(windows == 6, "[%s] only [%i] of its 6 faces have a window onto its core", label, windows);
}

void zox_tst_gravity_cube_model_check(ecs* world) {
    const entity* m = zox_tst_gcm_models;
    const char* labels[3] = { "seed A", "seed A again", "seed B" };
    for (byte i = 0; i < 3; i++) {
        zox_tst_gcm_check_cube(world, m[i], labels[i]);
    }
    const int same = zox_tst_gcm_differ(world, m[0], m[1]);
    zox_tst_check(!same && zox_tst_gcm_colors_equal(world, m[0], m[1]), "the same seed made another cube, [%i] voxels differ", same);
    zox_tst_check(!zox_tst_gcm_colors_equal(world, m[0], m[2]), "another seed made the same colours");
    zox_log("[tst] gravity cube: seed B's differs from A's in [%i] voxels", zox_tst_gcm_differ(world, m[0], m[2]));
}
