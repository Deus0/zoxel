void test_fall_through_terrain(ecs *world, int32_t keycode) {
    if (keycode != zox_key_p) {
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
    zox_log("- falling player character");
    zox_muter(character, Position3D, position);
    position->value.y -= 1.0f;
}
