byte sdl_event_background_entered(SDL_Event *event) {
    return event->type == SDL_EVENT_DID_ENTER_BACKGROUND;
}

byte sdl_event_background_left(SDL_Event *event) {
    return event->type == SDL_EVENT_DID_ENTER_FOREGROUND;
}

byte sdl_event_quit(SDL_Event *event) {
    return event->type == SDL_EVENT_QUIT;
}

byte sdl_event_window_resized(SDL_Event *event) {
    return event->type == SDL_EVENT_WINDOW_RESIZED
        || event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED;
}

byte sdl_event_display_orientation(SDL_Event *event) {
    return event->type == SDL_EVENT_DISPLAY_ORIENTATION;
}

byte sdl_event_window_moved(SDL_Event *event) {
    return event->type == SDL_EVENT_WINDOW_MOVED;
}

byte sdl_event_window_maximized(SDL_Event *event) {
    return event->type == SDL_EVENT_WINDOW_MAXIMIZED;
}

byte sdl_event_window_restored(SDL_Event *event) {
    return event->type == SDL_EVENT_WINDOW_RESTORED;
}

byte sdl_event_window_minimized(SDL_Event *event) {
    return event->type == SDL_EVENT_WINDOW_MINIMIZED;
}

byte sdl_event_window_focus_lost(SDL_Event *event) {
    return event->type == SDL_EVENT_WINDOW_FOCUS_LOST;
}

byte sdl_event_window_focus_gained(SDL_Event *event) {
    return event->type == SDL_EVENT_WINDOW_FOCUS_GAINED;
}

byte sdl_event_text_input(SDL_Event *event) {
    return event->type == SDL_EVENT_TEXT_INPUT;
}

int2 sdl_event_window_size(SDL_Event *event) {
    return (int2) {
        event->window.data1,
        event->window.data2
    };
}

const char *sdl_event_text(SDL_Event *event) {
    return event->text.text;
}