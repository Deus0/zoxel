
const float joystick_cutoff_buffer = 0.12f;
byte auto_switch_device = 1;
// the main menu's local coop join is up: A on a gamepad joins a player, it doesn't switch the first to it
//  (player_join_system sets it, device_switch_system reads it)
byte gamepad_a_joins = 0;
// local coop (two players or more): a player keeps its device, it switches only on its disconnect screen
//  (DeviceLost), a player's keyboard or a spare gamepad took the first one off its gamepad (player_join_system sets it)
byte device_switch_coop = 0;
