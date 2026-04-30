void MaterialDisposeSystem(iter *it) {
    zox_sys_begin();
    zox_sys_in(MaterialGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(MaterialGPULink, material);

#ifdef zox_log_gpu_management
        zox_log(" > disposing [%s] [%i]\n", zox_get_name(it->entities[i]), material->value)
#endif

        zox_dispose_material(material->value);
    }
} zoxd_system(MaterialDisposeSystem)
