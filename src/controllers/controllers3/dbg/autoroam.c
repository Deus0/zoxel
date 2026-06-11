extern void set_character3_npc(ecs* world, entity c, byte npc);

void toggle_autoroam(ecs *world) {
    /*if (keycode != zox_key_g) {
        return;
    }*/
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    zox_geter_value(player, CharacterLink, entity, c);
    if (!zox_valid(c)) {
        return;
    }
    byte autoplayer = !zox_has(c, Behaviour); // flips mode
    zox_log("Character Auto [%s]", autoplayer ? "Enabled" : "Disabled");
    set_character3_npc(world, c, autoplayer);
}
