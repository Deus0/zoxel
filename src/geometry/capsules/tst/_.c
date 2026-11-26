// tst: capsule_mesh, a capsule's mesh is the capsule's shape (pure, fun/mesh.c)
//  - radius 0.35, height 1, 16 segments: 16 round x 9 bands x 2 triangles, each vertex its own
//  - every vertex inside the capsule (within the radius of its axis, the axis half the height less the radius), its
//    lowest and highest at the ends, its widest at the radius; each triangle's colour its own, the front darker
//  - run: bash bsh/tst.sh capsule_mesh
byte zox_tst_capsule_mesh_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_capsule_mesh_check(ecs* world) {
    (void) world;
    const float radius = 0.35f, height = 1;
    MeshVertices vertices = { 0 };
    MeshIndicies indices = { 0 };
    MeshColors colors = { 0 };
    build_capsule_mesh(&vertices, &indices, &colors, radius, height, 16, (color_rgb) { 200, 100, 50 });
    zox_tst_check(vertices.length == 16 * 9 * 6 && indices.length == vertices.length && colors.length == vertices.length,
        "[%i] vertices [%i] indices [%i] colours, not [%i] each", vertices.length, indices.length, colors.length, 16 * 9 * 6);
    const float half_axis = height / 2 - radius;
    float lowest = 1, highest = -1, widest = 0, outside = 0;
    int dark = 0;
    for (int i = 0; i < vertices.length; i++) {
        const float3 v = vertices.value[i];
        lowest = fminf(lowest, v.y);
        highest = fmaxf(highest, v.y);
        widest = fmaxf(widest, sqrtf(v.x * v.x + v.z * v.z));
        const float3 axis = { 0, fmaxf(-half_axis, fminf(half_axis, v.y)), 0 };
        outside = fmaxf(outside, float3_length(float3_sub(v, axis)) - radius);
        dark += colors.value[i].r < 200;
    }
    zox_tst_check(fabsf(lowest + height / 2) < 0.01f && fabsf(highest - height / 2) < 0.01f,
        "its ends at [%f %f], not [%f %f]", lowest, highest, -height / 2, height / 2);
    zox_tst_check(fabsf(widest - radius) < 0.01f, "its widest [%f], not its radius [%f]", widest, radius);
    zox_tst_check(outside < 0.001f, "a vertex [%f] outside the capsule", outside);
    zox_tst_check(dark > 0 && dark < vertices.length / 4, "[%i] of [%i] vertices on its darker front", dark, vertices.length);
    dispose_MeshVertices(&vertices);
    dispose_MeshIndicies(&indices);
    dispose_MeshColors(&colors);
}

// tst: capsule_rebuild, a capsule mesh is built by its observer, and again when its shape changes (eve/_.c)
//  - spawned (radius 0.35, height 1): built, its widest the radius, MeshDirty to upload it
//  - its radius set to 0.5, Dirty added: built again, its widest 0.5
//  - run: bash bsh/tst.sh capsule_rebuild
int zox_tst_cr_stage = 0;
entity zox_tst_cr_capsule = 0;
float zox_tst_cr_widest[2];

static inline float zox_tst_cr_measure(ecs* world, entity e) {
    const MeshVertices* vertices = zox_get(e, MeshVertices);
    float widest = 0;
    for (int i = 0; i < vertices->length; i++) {
        widest = fmaxf(widest, sqrtf(vertices->value[i].x * vertices->value[i].x + vertices->value[i].z * vertices->value[i].z));
    }
    return widest;
}

byte zox_tst_capsule_rebuild_ready(ecs* world) {
    if (zox_tst_cr_stage == 0) {
        zox_tst_cr_capsule = spawn_capsule_mesh(world, 0, 0.35f, 1, (color_rgb) { 200, 100, 50 });
        zox_tst_cr_stage = 1;
        return 0;
    }
    zox_tst_waiting = "the capsule's mesh built";
    const entity e = zox_tst_cr_capsule;
    if (!zox_get(e, MeshVertices)->length) {
        return 0;
    }
    if (zox_tst_cr_stage == 1) {
        zox_tst_cr_widest[0] = zox_tst_cr_measure(world, e);
        zox_setv(e, RadiusFloat, 0.5f);
        zox_add(e, Dirty);
        zox_tst_cr_stage = 2;
        return 0;
    }
    zox_tst_cr_widest[1] = zox_tst_cr_measure(world, e);
    return 1;
}

void zox_tst_capsule_rebuild_check(ecs* world) {
    const entity e = zox_tst_cr_capsule;
    zox_tst_check(zox_get(e, MeshVertices)->length == capsule_mesh_segments * 9 * 6, "[%i] vertices, not [%i]",
        zox_get(e, MeshVertices)->length, capsule_mesh_segments * 9 * 6);
    zox_tst_check(fabsf(zox_tst_cr_widest[0] - 0.35f) < 0.01f, "built, its widest [%f], not 0.35", zox_tst_cr_widest[0]);
    zox_tst_check(fabsf(zox_tst_cr_widest[1] - 0.5f) < 0.01f, "rebuilt, its widest [%f], not 0.5", zox_tst_cr_widest[1]);
    zox_tst_check(zox_get_link(world, e, MaterialLink) == material_colored3D_lit, "it doesn't link the lit material");
}

void zox_tsts_capsules() {
    zox_tst_add("capsule_mesh", zox_tst_capsule_mesh_ready, zox_tst_capsule_mesh_check, 10);
    zox_tst_add("capsule_rebuild", zox_tst_capsule_rebuild_ready, zox_tst_capsule_rebuild_check, 10);
}
