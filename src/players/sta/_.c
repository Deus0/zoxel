#define zox_player_state_main_menu 0
// Startup Game
#define zox_player_state_loading 1      // Stream Begins / camera set
#define zox_player_state_starting 2     // Spawn starting character
// Entering Play
#define zox_player_state_play_begin 4   // For Resume and Start states
#define zox_player_state_playing 5
// TODO: Use a different state in respawning
#define zox_player_state_respawn_begin 30   // fade out
#define zox_player_state_respawning 31
#define zox_player_state_respawn 32
// Pause
#define zox_player_state_pause_begin 10
#define zox_player_state_paused 11
#define zox_player_state_resume_begin 12
// Dialogue
#define zox_player_state_dialogue_begin 21
#define zox_player_state_dialogue_active 22
#define zox_player_state_dialogue_end 23
