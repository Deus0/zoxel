void toggle_flymode(ecs *world, int32_t keycode) {
    if (keycode != zox_key_f) {
        return;
    }
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    entity character = zox_get_link(world, player, Character);
    if (!zox_valid(character)) {
        return;
    }
    zox_geter_value(character, FlyMode, byte, flying);
    flying = !flying;
    zox_set(character, FlyMode, { flying });
    if (flying && !zox_has(character, DisableGravity)) {
        zox_add(character, DisableGravity);
    } else if (!flying && zox_has(character, DisableGravity)) {
        zox_remove(character, DisableGravity);
    }
    // zox_set(character, DisableGravity, { flying });
    zox_log("Chacter FlyMode [%s]", flying ? "Enabled" : "Disabled");
}
