// #include <ctype.h> // for tolower function

static inline int zox_tolower(int c) {
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');
    }
    return c;
}

static byte zox_strcasestr(const char *haystack, const char *needle) {
    if (!*needle) return 1;
    for (; *haystack; ++haystack) {
        const char *h = haystack;
        const char *n = needle;
        while (*n && *h && zox_tolower((unsigned char)*h) == zox_tolower((unsigned char)*n)) {
            ++h; ++n;
        }
        if (!*n) return 1;
    }
    return 0;
}

byte is_on_phosh() {
    const char *desktop = getenv("XDG_CURRENT_DESKTOP");
    if (desktop && zox_strcasestr(desktop, "phosh")) {
        return 1;
    }

    const char *wayland = getenv("WAYLAND_DISPLAY");
    if (wayland && zox_strcasestr(wayland, "phoc")) {
        return 1;
    }

    return 0;
}
