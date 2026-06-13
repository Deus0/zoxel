entity prefab_material;

entity spawn_prefab_material(ecs *world) {
    zox_prefab();
    zox_prefab_name("material");
    zox_add_tag(e, Material);
    // zox_add(e, ShaderGPULink)
    add_gpu_material(world, e);
    prefab_material = e;
    return e;
}

entity spawn_material(ecs *world, entity shader, uint *material) {

    zox_instance(prefab_material);
    zox_name("material");
    zox_set(e, ShaderLink, { shader });

    uint2 shader_value = zox_get_value(shader, ShaderGPULink);

    *material = spawn_gpu_material(world, e, shader_value);

    if (!*material) {
        zox_geter(shader, ShaderSourceIndex, index);

        zox_logv("### [%s] ###", zox_get_name(shader));

        zox_logv("   .vert\n%s", shader_verts[index->value]);

        zox_logv("   .frag\n%s", shader_frags[index->value]);

         zox_logv("### ### ### ### ###");
    }

    return e;
}

void restore_material(ecs *world, entity e, uint2 shader) {
    uint material = spawn_gpu_material(world, e, shader);

    if (!material) {
        zox_log("[restore_material] Failed");
        return;
    }
}
