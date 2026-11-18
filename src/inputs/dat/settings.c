
const float joystick_cutoff_buffer = 0.12f;
byte auto_switch_device = 1;
// the main menu's local coop join is up: A on a gamepad joins a player, it doesn't switch the first to it
//  (player_join_system sets it, device_switch_system reads it)
byte gamepad_a_joins = 0;
