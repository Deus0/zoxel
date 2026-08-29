// NOTE: Pushes camera data onto material for Render systems
zox_sys2(CameraMaterialSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MaterialLink);
    zox_sys_in(CameraBlur);
    zox_sys_in(CameraVignette);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MaterialLink, material);
        zox_sys_i(CameraBlur, blur);
        zox_sys_i(CameraVignette, vignette);
        if (!zox_valid(material->value) ||
            !zox_has(material->value, MaterialBlur))
        {
            zox_logw("(CameraBlurSystem) Camera has invalid Material [%s]",
                zox_getn(e));
            continue;
        }
        zox_setv(material->value, CameraBlur, blur->value);
        zox_setv(material->value, CameraVignette, vignette->value);
        if (dbg_log) {
            zox_log("Camera [%s] Set Material [%s] to [%fx%f]", zox_getn(e), zox_getn(material->value), blur->value, vignette->value);
        }
    }
} zox_sys_end(CameraMaterialSystem);
