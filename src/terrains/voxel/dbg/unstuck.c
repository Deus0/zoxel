extern entity get_linked_character(ecs*, entity);

void debug_key_down_unstucking(ecs *world, int32_t keycode) {
    if (keycode != zox_key_b) {
        return;
    }

    entity player = dbg_player;
    if (!zox_valid(player)) return;

    entity character = get_linked_character(world, player);
    if (!character) return;

    zox_log("Character [%s] is being teleported down.", zox_get_name(character));

    zox_muter(character, Position3D, position);
    position->value.y -= 1;
}
