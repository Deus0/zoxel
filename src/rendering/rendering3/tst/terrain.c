// tst: terrain_render, a built terrain chunk mesh is drawn by chunk3_textured_render_system
//  - also checks the mesh has a uv per vertex, missing uvs render black/white
zox_tst_draws zox_tst_terrain_draws;

byte zox_tst_terrain_ready(ecs* world) {
    return zox_tst_draws_ready(
        world,
        &zox_tst_terrain_draws,
        zox_drawn_chunks,
        "a visible ChunkMesh with a MeshRenderCount");
}

void zox_tst_terrain_check(ecs* world) {
    zox_tst_draws* draws = &zox_tst_terrain_draws;
    entity e = draws->candidate;
    const uint drawn = zox_drawn_chunks - draws->baseline;
    zox_log("[tst] chunk mesh [%s], drawn [%u] chunks in [%.0fs]",
        zox_getn(e), drawn, zox_tst_draws_window);
    zox_tst_check(drawn, "no terrain chunks drawn in [%.0fs]", zox_tst_draws_window);
    if (zox_has(e, MeshUVs)) {
        const MeshUVs* uvs = zox_get(e, MeshUVs);
        const MeshVertices* vertices = zox_get(e, MeshVertices);
        zox_tst_check(uvs->length == vertices->length,
            "[%s] has [%i] uvs for [%i] vertices", zox_getn(e), uvs->length, vertices->length);
    }
    if (drawn) {
        return;
    }
    // same skips as chunk3_textured_render_system
    const guint2 mesh = zox_getv(e, MeshGPULink);
    zox_tst_check(mesh.x && mesh.y, "[%s] MeshGPULink [%u:%u]", zox_getn(e), mesh.x, mesh.y);
    zox_tst_check(zox_getv(e, UvsGPULink), "[%s] has no UvsGPULink", zox_getn(e));
    zox_tst_check(zox_getv(e, ColorsGPULink), "[%s] has no ColorsGPULink", zox_getn(e));
    zox_tst_check(!zox_has(e, Disabled), "[%s] is Disabled", zox_getn(e));
    entity material = zox_get_link(world, e, MaterialLink);
    zox_tst_check(zox_valid(material), "[%s] has no MaterialLink (tilemap)", zox_getn(e));
    if (zox_valid(material)) {
        zox_tst_check(zox_getv(material, MaterialGPULink), "tilemap [%s] has no MaterialGPULink", zox_getn(material));
        zox_tst_check(zox_getv(material, TextureGPULink), "tilemap [%s] has no TextureGPULink", zox_getn(material));
        zox_tst_check(zox_has(material, MaterialTextured3D), "tilemap [%s] has no MaterialTextured3D", zox_getn(material));
    }
    zox_tst_check_camera_filter(world, e);
}

void tst_terrain_render_probe_system(iter* it) {
    zox_sys_on_begin();
    if (!zox_tst_active || zox_tst_active->check != zox_tst_terrain_check) {
        return;
    }
    zox_sys_world();
    if (zox_valid(zox_tst_terrain_draws.candidate)) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(RenderDisabled);
    zox_sys_in(MeshRenderCount);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(MeshRenderCount, count);
        if (!disabled->value && count->value) {
            zox_tst_terrain_draws.candidate = e;
            return;
        }
    }
    zox_sys_on_end();
} zoxd_system(tst_terrain_render_probe_system);

void zox_tsts_terrain_render(ecs* world) {
    zox_tst_add("terrain_render", zox_tst_terrain_ready, zox_tst_terrain_check, 90);
    // main thread, in zoxp_render with the other main thread systems: no merges of its own (tst pipeline_syncs)
    zox_system_1(
        tst_terrain_render_probe_system,
        zoxp_render,
        [in] rendering.RenderDisabled,
        [in] rendering.MeshRenderCount,
        [none] rendering.ChunkMesh,
        [none] !core.Disabled
    );
}
