byte character_spawn_distance = 2;   // not used yet
byte character_spawn_rate_min = 0;
byte character_spawn_rate_max = 8; // set higher
byte disable_npc_movement = 0;

void key_down_toggle_npc_movement(ecs* world, int32_t keycode) {
    if (keycode != zox_key_F7) {
        return;
    }
    disable_npc_movement = !disable_npc_movement;
    zox_log("+ npc_movement is [%s]", disable_npc_movement ? "disabled" : "enabled");
}
