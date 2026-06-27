// MaxSystemTimeLabel
zox_sys2(MaxSystemTimeLabelSystem) {
    // Get the output
    zox_sys_world();
    // NOTE: For some reason we could not get the ID here, ecs namespacing?
    entity module = timing_module;
    // entity module = zox_id(Timing);
    if (!zox_valid(module)) {
        zox_loge("Timing Module not valid.");
        return;
    }
    entity max_system = zox_getv(module, SystemLink);
    if (!zox_valid(max_system)) {
        zox_loge("No Max System linked to Timing Module");
        return;
    }
    double max_delta = zox_getv(max_system, MaxDoubleData);
    char output[1024];
    output[0] = '\0';
    sprintf(output, "%s\nDelta Time [%0.01fms]", zox_get_name(max_system), max_delta);
    uint text_length = strlen(output);
    zox_sys_begin();
    zox_sys_out(TextData);
    zox_sys_out(TextDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(TextData, data);
        zox_sys_o(TextDirty, dirty);
        if (!is_zext_cut(data, output, text_length)) {
            set_zext_cut(data, output, text_length);
            dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(MaxSystemTimeLabelSystem);
