static inline byte is_skip_zigel(byte z) {
    return z == zox_char_newline
        || z == zox_char_space;
}

int get_zext_max_x(const byte *data, int length) {
    int x = 0;
    int max_x = 0;
    for (int i = 0; i < length; i++) {
        if (data[i] == zox_char_newline) {
            x = 0;
        } else {
            x++;
            if (x > max_x) {
                max_x = x;
            }
        }
    }
    if (x > max_x) {
        max_x = x;
    }
    return max_x;
}

int get_zext_max_y(const byte *data, int length) {
    int y = 1;
    for (int i = 0; i < length; i++) {
        if (data[i] == zox_char_newline && i != length - 1) {
            y++;
        }
    }
    return y;
}

int get_zext_x(const byte *data, int data_index) {
    int x = 0;
    for (int i = 0; i < data_index; i++) {
        if (data[i] == zox_char_newline) {
            x = 0;
        } else {
            x++;
        }
    }
    return x;
}

int get_zext_y(const byte *data, int length, int data_index) {
    int y = 0;
    for (int i = 0; i < data_index; i++) {
        if (data[i] == zox_char_newline && i != length - 1) y++;
    }
    return y;
}

uint calculate_total_zigels(const byte *data, int length) {
    uint count = 0;
    for (int i = 0; i < length; i++) {
        byte zigel = data[i];
        if (is_skip_zigel(zigel)) {
            continue;
        }
        count++;
    }
    return count;
}

// from a zigel (child?) index, get the data char code
byte calculate_zigel_index(const byte *data, int length, int child_index) {
    int j = 0;
    for (int i = 0; i < length; i++) {
        byte zigel = data[i];
        if (is_skip_zigel(zigel)) {
            continue;
        }
        if (j == child_index) {
            return data[i];
        }
        j++;
    }
    return 0;
}

int2 calculate_zext_size(const byte *data, int length, byte font_size, byte2 padding, byte line_padding) {
    int x = get_zext_max_x(data, length);
    int y = get_zext_max_y(data, length);
    // zox_log("sizey: %i\n", y)
    return (int2) { font_size * x + padding.x * 2, (font_size) * y + (y - 1) * line_padding + padding.y * 2 };
}

// wraps a text to a width in characters: a space becomes a new line where the next word would pass the line's end
//  - a word longer than a line stays whole (it overflows), new lines already in it start lines
void text_wrap(char* text, int columns) {
    if (!text || columns <= 0) {
        return;
    }
    int line_start = 0;
    int last_space = -1;
    for (int i = 0; text[i]; i++) {
        if (text[i] == '\n') {
            line_start = i + 1;
            last_space = -1;
            continue;
        }
        if (text[i] == ' ') {
            last_space = i;
        }
        if (i - line_start >= columns && last_space >= 0) {
            text[last_space] = '\n';
            line_start = last_space + 1;
            last_space = -1;
        }
    }
}

// how many characters fit across a text ui: its LayoutSize less its padding (glyphs are font size wide)
int text_columns(ecs* world, entity e) {
    const byte font_size = zox_getv(e, TextFontSize);
    if (!font_size || !zox_has(e, LayoutSize)) {
        return 0;
    }
    return (zox_getv(e, LayoutSize).x - zox_getv(e, TextPadding).x * 2) / font_size;
}

// how many lines fit down a text ui: its LayoutSize less its padding (lines are font size tall, line padding between)
int text_rows(ecs* world, entity e) {
    const byte font_size = zox_getv(e, TextFontSize);
    if (!font_size || !zox_has(e, LayoutSize)) {
        return 0;
    }
    return (zox_getv(e, LayoutSize).y - zox_getv(e, TextPadding).y * 2 + default_line_padding) / (font_size + default_line_padding);
}

// cuts a text to its first rows lines, the last ending in ... if it was cut (a box too short for it)
void text_truncate(char* text, int rows) {
    if (!text || rows <= 0) {
        return;
    }
    int line = 0, line_start = 0;
    for (int i = 0; text[i]; i++) {
        if (text[i] != '\n') {
            continue;
        }
        if (++line < rows) {
            line_start = i + 1;
            continue;
        }
        if (!text[i + 1]) {
            return; // a trailing new line, nothing past it
        }
        // text[i] ends the last line that fits: its last three characters become ... (after it if it's short, the
        //  text went on past it so there's room)
        const int dots = i - 3 >= line_start ? i - 3 : i;
        text[dots] = text[dots + 1] = text[dots + 2] = '.';
        text[dots + 3] = 0;
        return;
    }
}
