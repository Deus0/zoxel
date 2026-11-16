// tst: sdl_text_input, typing (sdl's text input event, through sdl_events_system) reaches a focused input field
//  - input_field (_.c) types with add_text_typed, this is the step before it: sdl's event
//  - run: bash bsh/tst.sh sdl_text_input
void zox_tst_sdl_text_push(ecs* world, SDL_Event* event) {
    SDL_Window* window = zox_valid(main_app) ? zox_getv(main_app, SDLWindow) : NULL;
    event->window.windowID = window ? SDL_GetWindowID(window) : 0;
    SDL_PushEvent(event);
}

int zox_tst_sdl_text_frames = 0;
int zox_tst_sdl_text_stage = 0;

entity zox_tst_sdl_field = 0;
char zox_tst_sdl_field_value[64];

byte zox_tst_sdl_text_input_ready(ecs* world) {
    const entity canvas = zox_valid(dbg_player) ? zox_get_link(world, dbg_player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_sdl_text_frames++;
    if (zox_tst_sdl_text_stage == 0) {
        zox_tst_waiting = "a few frames";
        if (zox_tst_sdl_text_frames < 10) {
            return 0;
        }
        // an input field (as input_field)
        zox_tst_sdl_field = spawn_button(world, prefab_button, canvas, "Realm", int2_zero, int2_zero, float2_half,
            zox_alignment_centre, 24, (byte2) { 6, 4 }, button_fill, button_outline, button_font_fill, button_font_outline).x;
        zox_add(zox_tst_sdl_field, TextInput);
        zox_setv(zox_tst_sdl_field, ClickEvent, &on_click_input_field);
        zox_tst_sdl_text_stage = 1;
        zox_tst_sdl_text_frames = 0;
        return 0;
    }
    if (zox_tst_sdl_text_stage == 1) {
        zox_tst_waiting = "the field";
        if (zox_tst_sdl_text_frames < 5) {
            return 0;
        }
        on_click_input_field(world, (ClickEventData) { .clicker = dbg_player, .clicked = zox_tst_sdl_field });
        // typed: sdl's text event, through sdl_events_system
        SDL_Event event = { 0 };
        event.type = SDL_EVENT_TEXT_INPUT;
        event.text.text = " Zox";
        zox_tst_sdl_text_push(world, &event);
        zox_tst_sdl_text_stage = 2;
        zox_tst_sdl_text_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the field to take the typing";
    char* value = input_field_value(world, zox_tst_sdl_field);
    snprintf(zox_tst_sdl_field_value, sizeof(zox_tst_sdl_field_value), "%s", value);
    free(value);
    if (strcmp(zox_tst_sdl_field_value, "Realm Zox") && zox_tst_sdl_text_frames < 30) {
        return 0;
    }
    input_field_unfocus(world);
    return 1;
}

void zox_tst_sdl_text_input_check(ecs* world) {
    (void) world;
    zox_tst_check(!strcmp(zox_tst_sdl_field_value, "Realm Zox"), "an sdl text event (typing \" Zox\") didn't reach the focused field");
    if (strcmp(zox_tst_sdl_field_value, "Realm Zox")) {
        zox_log("[tst] the field has [%s]", zox_tst_sdl_field_value);
    }
}
