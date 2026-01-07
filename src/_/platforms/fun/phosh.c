
static byte strcasestr(const char *haystack, const char *needle) {
    if (!*needle) return 1;
    for (; *haystack; ++haystack) {
        const char *h = haystack;
        const char *n = needle;
        while (*n && *h && tolower((unsigned char)*h) == tolower((unsigned char)*n)) {
            ++h; ++n;
        }
        if (!*n) return 1;
    }
    return 0;
}

byte is_on_phosh() {
    const char *desktop = getenv("XDG_CURRENT_DESKTOP");
    if (desktop && strcasestr(desktop, "phosh")) {
        return 1;
    }

    const char *wayland = getenv("WAYLAND_DISPLAY");
    if (wayland && strcasestr(wayland, "phoc")) {
        return 1;
    }

    return 0;
}