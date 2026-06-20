// NOTE: Updates the alpha of the block damage overlays
zox_sys2(BlockHealthOverlaySystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StatValue);
    zox_sys_in(StatValueMax);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(StatValue, value);
        zox_sys_i(StatValueMax, max);
        if (value->value == max->value) {
            if (dbg_log) {
                zox_log("World Block is Max Health");
            }
            continue;
        }
        entity overlay = zox_get_child_by_id(world, e, zox_id(Cube));
        if (!zox_valid(overlay)) {
            zox_loge("World Block missing a Overlay.");
            continue;
        }
        zox_muter(overlay, Alpha, alpha);
        float new_alpha = (max->value - value->value) / max->value;
        if (alpha->value != new_alpha) {
            alpha->value = new_alpha;
            if (dbg_log) {
                zox_log("Block Health Alpha Set [%f]", new_alpha);
            }
        }
    }
} zox_sys_end(BlockHealthOverlaySystem);
