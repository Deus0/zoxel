// tst: vox_instance_render, a visible instance renderer is drawn by vox_instance_render_system
zox_tst_draws zox_tst_vox_instance_draws;

byte zox_tst_vox_instance_ready(ecs* world) {
    return zox_tst_draws_ready(
        world,
        &zox_tst_vox_instance_draws,
        zox_drawn_vox_instances,
        "a visible RendererInstance with an InstanceLink");
}

void zox_tst_vox_instance_check(ecs* world) {
    zox_tst_draws* draws = &zox_tst_vox_instance_draws;
    entity e = draws->candidate;
    entity mesh = zox_get_link(world, e, InstanceLink);
    const uint drawn = zox_drawn_vox_instances - draws->baseline;
    zox_log("[tst] vox instance [%s] mesh [%s], drawn [%u] instances in [%.0fs]",
        zox_getn(e), zox_getn(mesh), drawn, zox_tst_draws_window);
    zox_tst_check(drawn, "no vox instances drawn in [%.0fs]", zox_tst_draws_window);
    if (drawn) {
        return;
    }
    // same skips as vox_instance_render_system
    zox_tst_check(zox_valid(material_vox_instance), "material_vox_instance invalid");
    if (zox_valid(material_vox_instance)) {
        zox_tst_check(zox_getv(material_vox_instance, MaterialGPULink),
            "material_vox_instance has no MaterialGPULink");
        zox_tst_check(zox_getv(material_vox_instance, UboGPULink),
            "material_vox_instance has no UboGPULink");
    }
    zox_tst_check(zox_valid(mesh), "[%s] InstanceLink invalid", zox_getn(e));
    if (zox_valid(mesh)) {
        zox_tst_check(zox_has(mesh, MeshIndicies), "instance mesh [%s] has no MeshIndicies", zox_getn(mesh));
        if (zox_has(mesh, MeshIndicies)) {
            zox_geter(mesh, MeshIndicies, indicies);
            zox_tst_check(indicies->length, "instance mesh [%s] has 0 indicies", zox_getn(mesh));
        }
        zox_tst_check(zox_has(mesh, MeshGPULink), "instance mesh [%s] has no MeshGPULink", zox_getn(mesh));
        if (zox_has(mesh, MeshGPULink)) {
            const guint2 gpu = zox_getv(mesh, MeshGPULink);
            zox_tst_check(gpu.x && gpu.y, "instance mesh [%s] MeshGPULink [%u:%u]", zox_getn(mesh), gpu.x, gpu.y);
        }
        zox_tst_check(zox_has(mesh, ColorsGPULink) && zox_getv(mesh, ColorsGPULink),
            "instance mesh [%s] has no ColorsGPULink", zox_getn(mesh));
    }
    zox_tst_check_camera_filter(world, e);
}

zox_sys2(TstVoxInstanceProbeSystem) {
    if (!zox_tst_active || zox_tst_active->check != zox_tst_vox_instance_check) {
        return;
    }
    zox_sys_world();
    if (zox_valid(zox_tst_vox_instance_draws.candidate)) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(RenderDisabled);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDisabled, disabled);
        if (!disabled->value && zox_valid(zox_get_link(world, e, InstanceLink))) {
            zox_tst_vox_instance_draws.candidate = e;
            return;
        }
    }
} zox_sys_end(TstVoxInstanceProbeSystem);

void zox_tsts_vox_instance_render(ecs* world) {
    zox_tst_add("vox_instance_render", zox_tst_vox_instance_ready, zox_tst_vox_instance_check, 90);
    zox_system_1(
        TstVoxInstanceProbeSystem,
        zoxp_update,
        [in] rendering.RenderDisabled,
        [none] rendering.RendererInstance
    );
}
