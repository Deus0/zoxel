byte sdl_event_background_entered(SDL_Event *event) {
    return event->type == SDL_APP_DIDENTERBACKGROUND;
}

byte sdl_event_background_left(SDL_Event *event) {
    return event->type == SDL_APP_DIDENTERFOREGROUND;
}

byte sdl_event_quit(SDL_Event *event) {
    return event->type == SDL_QUIT;
}

byte sdl_event_window_resized(SDL_Event *event) {
    return event->type == SDL_WINDOWEVENT &&
        (event->window.event == SDL_WINDOWEVENT_RESIZED ||
         event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED);
}

byte sdl_event_display_orientation(SDL_Event *event) {
    return event->type == SDL_DISPLAYEVENT &&
        event->display.event == SDL_DISPLAYEVENT_ORIENTATION;
}

byte sdl_event_window_moved(SDL_Event *event) {
    return event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_MOVED;
}

byte sdl_event_window_maximized(SDL_Event *event) {
    return event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_MAXIMIZED;
}

byte sdl_event_window_restored(SDL_Event *event) {
    return event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_RESTORED;
}

byte sdl_event_window_minimized(SDL_Event *event) {
    return event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_MINIMIZED;
}

byte sdl_event_window_focus_lost(SDL_Event *event) {
    return event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_FOCUS_LOST;
}

byte sdl_event_window_focus_gained(SDL_Event *event) {
    return event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_FOCUS_GAINED;
}

byte sdl_event_text_input(SDL_Event *event) {
    return event->type == SDL_TEXTINPUT;
}

int2 sdl_event_window_size(SDL_Event *event) {
    return (int2) {
        event->window.data1,
        event->window.data2
    };
}

sbyte sdl_event_get_monitor(SDL_Event *event) {
    return (sbyte) event->display.display;
}

const char *sdl_event_text(SDL_Event *event) {
    return event->text.text;
}