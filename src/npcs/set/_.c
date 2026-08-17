byte character_spawn_distance = 2;
byte2 character_spawn_distance_limits = (byte2) { 0, 4 };
byte character_spawn_rate_min = 0;
byte character_spawn_rate_max = 3;
byte2 character_spawn_rate_limits = (byte2) { 0, 16 };
byte disable_npc_movement = 0;

void key_down_toggle_npc_movement(ecs* world, int32_t keycode) {
    if (keycode != zox_key_F7) {
        return;
    }
    disable_npc_movement = !disable_npc_movement;
    zox_log("+ npc_movement is [%s]", disable_npc_movement ? "disabled" : "enabled");
}
