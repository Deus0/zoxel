void test_fall_through_terrain(ecs *world, int32_t keycode) {
    if (keycode != SDLK_p) {
        return;
    }

    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }

    zox_geter(player, CharacterLink, characterLink);
    if (!zox_valid(characterLink->value)) {
        return;
    }

    zox_log("- falling player character");
    zox_muter(characterLink->value, Position3D, position3D)
    position3D->value.y -= 1.0f;
}
