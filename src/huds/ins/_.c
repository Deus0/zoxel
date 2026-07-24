const byte zox_huds_header_font_size = 22;
const byte zox_huds_element_font_size = 10;
const byte zox_huds_list_padding = 8;
const float2 zox_huds_window_anchor = (float2) { 0.15f, 0.85f }; // float2_top_left; // float2_centre;
const byte zox_huds_window_alignment = zox_alignment_centre;
// Main
#include "start.c"
#include "realm.c"
#include "realm_new.c"
#include "realm_pick.c"
#include "settings.c"
#include "main.c"
// Game
#include "taskbar.c"
#include "touch.c"
#include "pause.c"
#include "respawn.c"
// Editor
#include "terminal.c"
