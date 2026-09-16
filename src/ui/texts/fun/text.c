char* get_zext_text(const TextData *data) {
    return convert_zext_to_text(data->value, data->length);
}

byte is_zext(TextData* zext, const char* text) {
    if (!zext || !zext->value) {
        return 0; // error
    }
    int length = text ? strlen(text) : 0;
    if (zext->length != length) {
        return 0;
    }
    for (int i = 0; i < length; i++) {
        byte j = convert_ascii(text[i]);
        byte k = zext->value[i];
        if (j != k) {
            return 0;
        }
    }
    return 1;
}

void set_zext(
    TextData* text,
    const char* ntext)
{
    if (!ntext) {
        resize_TextData(text, 0);
        return;
    }
    int length = strlen(ntext);
    if (text->length != length) {
        resize_TextData(text, length);
    }
    for (uint i = 0; i < length; i++) {
        char ascii_char = ntext[i];
        text->value[i] = convert_ascii(ascii_char);
    }
}

byte set_text_component(
    TextData* text,
    const char* ntext)
{
    if (is_zext(text, ntext)) {
        return 0;
    } else {
        set_zext(text, ntext);
        return 1;
    }
}

void print_entity_zext(
    ecs *world,
    entity e)
{
    if (!zox_has(e, TextData)) {
        return;
    }
    zox_geter(e, TextData, data);
    char *debug_text = get_zext_text(data);
    zox_log("   > zext %lu [%s] length %i\n", e, debug_text, data->length)
    free(debug_text);
}

static inline byte set_entity_text(
    ecs* world,
    entity e,
    const char* text)
{
    if (!zox_valid(e) ||
        !zox_has(e, TextData) ||
        !zox_has(e, TextDirty))
    {
        zox_loge("invalid zext in [set_entity_text]")
        return 0;
    }
    zox_muter(e, TextData, text_data);
    if (!is_zext(text_data, text)) {
        set_zext(text_data, text);
        zox_setv(e, TextDirty, zox_dirty_trigger);
        return 1;
    } else {
        return 0;
    }
}

void set_entity_text_raw(ecs* world, entity e, const char* text) {
    TextData data = (TextData) { 0 };
    set_zext(&data, text);
    zox_set_ptr(e, TextData, data);
    zox_set(e, TextDirty, { zox_dirty_trigger });
}

int get_zexts_count(ecs *world) {
    return zox_count_types(Text);
}

byte is_zext_cut(TextData *zext, const char* text, byte cut_length) {
    if (!zext || !zext->value) {
        return 0; // error
    }
    if (zext->length != cut_length) {
        return 0;
    }
    if (!cut_length && !zext->length) {
        return 1;
    }
    for (int i = 0; i < cut_length; i++) {
        byte j = convert_ascii(text[i]);
        byte k = zext->value[i];
        if (j != k) {
            return 0;
        }
    }
    return 1;
}

void set_zext_cut(TextData *data, const char* text, byte cut_length) {
    if (data->length != cut_length) {
        resize_memory_component(TextData, data, byte, cut_length);
    }
    for (int i = 0; i < cut_length; i++) {
        data->value[i] = convert_ascii(text[i]);
    }
}
