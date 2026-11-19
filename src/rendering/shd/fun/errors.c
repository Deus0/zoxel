gint shader_uniform(guint program, const char *name, const char *shader_name) {
    const gint location = zox_gpu_get_material_property(program, name);
    if (location == -1) {
        zox_loge("(%s) uniform [%s] not found (renamed, misspelled, or unused and stripped)", shader_name, name);
        zox_gpu_log_program_inputs(program, shader_name);
    }
    return location;
}

gint shader_attribute(guint program, const char *name, const char *shader_name) {
    const gint location = zox_gpu_get_material_attribute(program, name);
    if (location == -1) {
        zox_loge("(%s) attribute [%s] not found (renamed, misspelled, or unused and stripped)", shader_name, name);
        zox_gpu_log_program_inputs(program, shader_name);
    }
    return location;
}
