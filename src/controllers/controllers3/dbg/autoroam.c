extern void set_character3_npc(ecs* world, entity c, byte npc);

void toggle_autoroam(ecs *world) {
    /*if (keycode != zox_key_g) {
        return;
    }*/
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    entity character = zox_get_link(world, player, CharacterLink);
    if (!zox_valid(character)) {
        return;
    }
    byte autoplayer = !zox_has(character, Behaviour); // flips mode
    zox_log("Character Auto [%s]",
        autoplayer ?
            "Enabled" :
            "Disabled");
    set_character3_npc(world, character, autoplayer);
}
