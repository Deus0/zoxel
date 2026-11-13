static inline char* get_zext_text(const TextData *data) {
    return convert_zext_to_text(
        data->value,
        data->length);
}

static inline byte is_zext(
    TextData* zext,
    const char* text)
{
    if (!zext) {
        return 0; // error
    }
    int length = text ?
        strlen(text) :
        0;
    if (zext->length != length) {
        return 0;
    }
    // both empty
    if (!length) {
        return 1;
    }
    if (!zext->value) {
        zox_loge("Text value is invalid in [is_zext]: %s, Lengths [%i] - [%i]",
            text,
            zext->length,
            length);
        return 0; // error
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
    // zox_log("set_zext [%s]", ntext);
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
    const TextData* data = zox_get(e, TextData);
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
        !zox_has(e, TextData))
    {
        zox_loge("invalid Text in [set_entity_text]: [%s]",
            zox_getn(e));
        return 0;
    }
    zox_muter(e, TextData, text_data);
    if (!is_zext(text_data, text)) {
        set_zext(text_data, text);
        zox_add(e, Dirty);
        return 1;
    } else {
        return 0;
    }
}

int get_zexts_count(ecs *world) {
    return zox_count_types(Text);
}

// NOTE: lengths are uint, dialogue lines are longer than a byte (TargetText is 512)
byte is_zext_cut(
    TextData *zext,
    const char* text,
    uint cut_length)
{
    if (!zext) {
        return 0; // error
    }
    if ((uint) zext->length != cut_length) {
        return 0;
    }
    // both empty, an empty text has no value
    if (!cut_length) {
        return 1;
    }
    if (!zext->value) {
        return 0; // error
    }
    for (uint i = 0; i < cut_length; i++) {
        byte j = convert_ascii(text[i]);
        byte k = zext->value[i];
        if (j != k) {
            return 0;
        }
    }
    return 1;
}

void set_zext_cut(
    TextData *data,
    const char* text,
    uint cut_length)
{
    if (data->length != cut_length) {
        resize_memory_component(TextData, data, byte, cut_length);
    }
    for (uint i = 0; i < cut_length; i++) {
        data->value[i] = convert_ascii(text[i]);
    }
}
