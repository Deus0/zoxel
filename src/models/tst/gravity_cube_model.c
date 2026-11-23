// tst: gravity_cube_model, the gravity cube's blueprint (models/ins/block_blueprint.c), its frames at depth 5
//  - three sets of its two VoxAnimations (spawn_blueprint_animation: off, and on), gravity_cube_frames frames each:
//    seed A, seed A again, seed B; each animation a VoxAnimation, its children its frames
//  - the same seed makes the same frames (voxels and colours), another seed other colours
//  - each frame: its 7 colours, the metal dark (value under 30) and the neon bright and saturated (over 80); its 8
//    corners neon (an edge, a trace or a light over it); a window on each of its 6 faces (air at the face's middle
//    through its outer layer) onto its core
//  - its lights chase: along an edge they're a step further each frame (frame f + 1's lights are frame f's moved one
//    on), round to the start after the last (its corners left out: the other edges there light them too)
//  - its rune (voxel 6) only on the on frames, the same on each
//  - to look at it: zox_tst_model=gravity zox_tst_seed=<seed> bash bsh/tst.sh model_views
//  - run: bash bsh/tst.sh gravity_cube_model
#define zox_tst_gcm_sets 3
#define zox_tst_gcm_frames (gravity_cube_frames * 2)
entity zox_tst_gcm_groups[zox_tst_gcm_sets][2];     // off, on
entity zox_tst_gcm_models[zox_tst_gcm_sets][zox_tst_gcm_frames];
int zox_tst_gcm_wait = 0;

byte zox_tst_gravity_cube_model_ready(ecs* world) {
    if (!zox_tst_gcm_groups[0][0]) {
        const entity holder = zox_new();
        const entity graph = spawn_model_nodegraph_gravity_cube(world, holder);
        const lint seeds[zox_tst_gcm_sets] = { 666, 666, 4242 };
        const char* names[zox_tst_gcm_sets] = { "tst_cube_a", "tst_cube_a2", "tst_cube_b" };
        for (byte i = 0; i < zox_tst_gcm_sets; i++) {
            for (byte on = 0; on < 2; on++) {
                zox_tst_gcm_groups[i][on] = spawn_blueprint_animation(world, holder, graph, seeds[i], names[i], block_depth_limits.y,
                    gravity_cube_frames, gravity_cube_fps, on).x;
            }
        }
        return 0;
    }
    zox_tst_waiting = "the gravity cube's frames to generate";
    if (++zox_tst_gcm_wait < 3) {
        return 0;
    }
    for (byte i = 0; i < zox_tst_gcm_sets; i++) for (byte on = 0; on < 2; on++) {
        const entity animation = zox_tst_gcm_groups[i][on];
        if (!zox_has(animation, VoxAnimation)) {
            return 0;
        }
        entity models[64];
        const uint count = zox_get_children_by_id(world, animation, models, 64, zox_id(Model));
        if (count != gravity_cube_frames) {
            return 0;
        }
        for (uint j = 0; j < count; j++) {
            if (zox_has(models[j], Generating)) {
                return 0;
            }
            zox_tst_gcm_models[i][zox_getv(models[j], ModelFrame) + on * gravity_cube_frames] = models[j];
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

static inline int zox_tst_gcm_count(ecs* world, entity model, byte voxel) {
    const int length = octree_size(zox_getv(model, NodeDepth));
    int count = 0;
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        count += zox_tst_gcm_voxel(world, model, x, y, z) == voxel;
    }
    return count;
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

static inline void zox_tst_gcm_check_frame(ecs* world, entity model, const char* label) {
    const int length = octree_size(zox_getv(model, NodeDepth));
    const ColorRGBs* colors = zox_get(model, ColorRGBs);
    zox_tst_check(colors->length == 7, "[%s] has [%i] colours, not metal 2, neon 2, core, rune and lights", label, colors->length);
    if (colors->length >= 3) {
        const float3 metal = color_rgb_to_hsv(colors->value[0]);
        const float3 neon = color_rgb_to_hsv(colors->value[2]);
        zox_tst_check(metal.z < 30, "[%s] its metal isn't dark, value [%.0f]", label, metal.z);
        zox_tst_check(neon.y > 80 && neon.z > 80, "[%s] its neon isn't bright, saturation [%.0f] value [%.0f]", label, neon.y, neon.z);
    }
    int corners = 0;
    for (int c = 0; c < 8; c++) {
        const byte voxel = zox_tst_gcm_voxel(world, model, c & 1 ? length - 1 : 0, c & 2 ? length - 1 : 0, c & 4 ? length - 1 : 0);
        corners += voxel == 3 || voxel == 4 || voxel == 7;
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
        windows += outer_air && inside >= 4 && inside <= 6;    // the core, a trace on it, the rune on it
    }
    zox_tst_check(windows == 6, "[%s] only [%i] of its 6 faces have a window onto its core", label, windows);
}

// lights along the edge x 0 to the end at y 0, z 0, a bit each
static inline uint zox_tst_gcm_edge_lights(ecs* world, entity model) {
    const int length = octree_size(zox_getv(model, NodeDepth));
    uint lights = 0;
    for (int x = 0; x < length && x < 32; x++) {
        if (zox_tst_gcm_voxel(world, model, x, 0, 0) == 7) {
            lights |= 1u << x;
        }
    }
    return lights;
}

void zox_tst_gravity_cube_model_check(ecs* world) {
    char label[64];
    for (byte i = 0; i < zox_tst_gcm_sets; i++) {
        for (byte f = 0; f < zox_tst_gcm_frames; f++) {
            snprintf(label, sizeof(label), "set %i frame %i", i, f);
            zox_tst_gcm_check_frame(world, zox_tst_gcm_models[i][f], label);
        }
    }
    entity* a = zox_tst_gcm_models[0];
    int same = 0;
    for (byte f = 0; f < zox_tst_gcm_frames; f++) {
        same += zox_tst_gcm_differ(world, a[f], zox_tst_gcm_models[1][f]);
    }
    zox_tst_check(!same && zox_tst_gcm_colors_equal(world, a[0], zox_tst_gcm_models[1][0]), "the same seed made other frames, [%i] voxels differ", same);
    zox_tst_check(!zox_tst_gcm_colors_equal(world, a[0], zox_tst_gcm_models[2][0]), "another seed made the same colours");
    // the lights chase: frame f + 1's along the edge are frame f's a step back (k + frame lit), round to the start
    for (byte f = 0; f < gravity_cube_frames; f++) {
        const uint now = zox_tst_gcm_edge_lights(world, a[f]);
        const uint next = zox_tst_gcm_edge_lights(world, a[(f + 1) % gravity_cube_frames]);
        zox_tst_check(now != 0, "frame [%i] has no lights along its edge", f);
        const uint step = now >> 1;                 // a step back along the 32 voxel edge
        const uint inside = 0x3FFFFFFEu;            // its corners (and what shifts in from one) left out: the edges
                                                    //  meeting there light them too
        zox_tst_check((next & inside) == (step & inside), "frame [%i]'s lights [%08x] aren't frame [%i]'s [%08x] a step on", (f + 1) % gravity_cube_frames,
            next, f, now);
    }
    // the rune: on the on frames only, the same on each
    const int rune = zox_tst_gcm_count(world, a[gravity_cube_frames], 6);
    zox_log("[tst] gravity cube: rune [%i] voxels, seed B's frame 0 differs from A's in [%i]", rune,
        zox_tst_gcm_differ(world, a[0], zox_tst_gcm_models[2][0]));
    zox_tst_check(rune > 0, "no rune on its on frame");
    for (byte f = 0; f < zox_tst_gcm_frames; f++) {
        const int count = zox_tst_gcm_count(world, a[f], 6);
        zox_tst_check(f < gravity_cube_frames ? count == 0 : count == rune, "frame [%i] has [%i] rune voxels, expected [%i]",
            f, count, f < gravity_cube_frames ? 0 : rune);
    }
}
