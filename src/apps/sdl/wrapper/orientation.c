byte get_screen_orientation(byte screen_index) {
    SDL_DisplayOrientation orientation = SDL_GetDisplayOrientation(screen_index);
    if (orientation == SDL_ORIENTATION_LANDSCAPE) {
        return 0;
    } else if (orientation == SDL_ORIENTATION_LANDSCAPE_FLIPPED) {
        return 1;
    } else if (orientation == SDL_ORIENTATION_PORTRAIT) {
        return 2;
    } else if (orientation == SDL_ORIENTATION_PORTRAIT_FLIPPED) {
        return 3;
    } else {
        return 0; // fallback
    }
}