// tst: font_glyphs, draws the font's symbols big on the main menu and saves a screenshot
//  - for looking at glyphs (i.e. ? and ! for quest markers), the test only checks it saved
//  - bash bsh/tst.sh font_glyphs, then look at bin/tst/font_glyphs.ppm
extern char zox_screenshot_request[256];
int zox_tst_font_stage = 0;
int zox_tst_font_frames = 0;

byte zox_tst_font_glyphs_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_font_frames++;
    if (zox_tst_font_stage == 0) {
        zox_tst_waiting = "the main menu";
        const entity main_menu = zox_tst_main_menu(world, player, canvas);
        if (!main_menu || zox_tst_font_frames < 10) {
            return 0;
        }
        zox_delete(main_menu);
        spawn_label(world, prefab_label2t, canvas, int2_zero, float2_half, byte2_single(8),
            "? ! + = Ab", 48, zox_alignment_centre, 200,
            button_fill, button_outline, button_font_fill, button_font_outline);
        zox_tst_font_stage = 1;
        zox_tst_font_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the screenshot";
    if (zox_tst_font_stage == 1) {
        if (zox_tst_font_frames < 20) {
            return 0;
        }
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "../tst/font_glyphs.ppm");
        zox_tst_font_stage = 2;
        return 0;
    }
    return !zox_screenshot_request[0];
}

void zox_tst_font_glyphs_check(ecs* world) {
    (void) world;
    FILE* file = fopen("../tst/font_glyphs.ppm", "rb");
    zox_tst_check(file, "no screenshot saved");
    if (file) {
        fclose(file);
    }
}
