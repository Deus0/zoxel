extern void set_character3_npc(ecs* world, entity c, byte npc);

void toggle_autoroam(ecs *world, int32_t keycode) {
    if (keycode == SDLK_g) {
        if (!zox_valid(local_realm) || !zox_has(local_realm, GameLink)) {
            zox_log_error("no realm (local)")
            return;
        }
        zox_geter(local_realm, GameLink, gameLink)
        if (!zox_valid(gameLink->value)) {
            zox_log_error("realm has no game")
            return;
        }
        zox_geter(gameLink->value, PlayerLinks, players)
        const entity player = players->value[0];
        zox_geter_value(player, CharacterLink, entity, c);
        if (!zox_valid(c)) {
            return;
        }

        byte autoplayer = !zox_has(c, Behaviour); // flips mode
        zox_log("Character Auto [%s]", autoplayer ? "Enabled" : "Disabled");
        set_character3_npc(world, c, autoplayer);
    }
}