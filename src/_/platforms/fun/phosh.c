byte is_on_phosh() {
    const char *desktop = getenv("XDG_CURRENT_DESKTOP");
    if (desktop && strstr(desktop, "phosh")) {
        return 1;
    }

    const char *wayland_display = getenv("WAYLAND_DISPLAY");
    if (wayland_display && strstr(wayland_display, "phoc")) {
        return 1;
    }

    return 0;
}