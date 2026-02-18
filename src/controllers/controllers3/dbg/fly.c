void toggle_flymode(ecs *world, int32_t keycode) {
    if (keycode != SDLK_f) {
        return;
    }

    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }

    zox_geter_value(player, CharacterLink, entity, character);
    if (!zox_valid(character)) {
        return;
    }

    zox_geter_value_non_const(character, FlyMode, byte, flying);
    flying = !flying;
    zox_set(character, FlyMode, { flying });
    zox_set(character, DisableGravity, { flying });
    zox_log("Chacter FlyMode [%s]", flying ? "Enabled" : "Disabled");
}
