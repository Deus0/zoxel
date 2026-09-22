// todo: rewrite this test (aura now on skill not player character)
void key_down_test_aura(ecs *world, int32_t keycode) {
    if (keycode == zox_key_y) {
        entity player = zox_players[0];
        if (!player) {
            zox_log("! player is null")
            return;
        }
        entity character = zox_get_link(world, player, CharacterLink);
        if (!zox_valid(character)) {
            zox_log("! character is null\n")
            return;
        }
        if (zox_has(character, Aura)) {
            zox_remove(character, Aura);
            zox_log(" > removed Aura from character\n")
        } else {
            zox_add(character, Aura);
            zox_log(" > added Aura to character\n")
        }
    }
}
