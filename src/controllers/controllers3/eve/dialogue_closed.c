// a dialogue window gone (after it pops out, dialogue_end_system): its player gets input back, other windows show again
//  - only from an active dialogue, a window deleted as a game ends leaves the player's state alone
void on_removed_dialogue_ui(iter* it) {
    ecs* world = it->world;
    for (int i = 0; i < it->count; i++) {
        const entity canvas = zox_get_parent(world, it->entities[i]);
        const entity player = zox_valid(canvas) ? zox_get_link(world, canvas, PlayerLink) : 0;
        if (!zox_valid(player) || zox_getv(player, PlayerState) != zox_player_state_dialogue_active) {
            continue;
        }
        zox_setv(player, PlayerState, zox_player_state_dialogue_end);
        entity windows[zox_children_capacity];
        const uint length = zox_get_children_by_id(world, canvas, windows, zox_children_capacity, zox_id(Window));
        for (uint j = 0; j < length; j++) {
            if (windows[j] != it->entities[i]) {
                set_children_by_id_byte(world, windows[j], zox_id(RenderDisabled), 0);
            }
        }
    }
}
