// #define zoxel_debug_zext_updates

void set_zox_name(ZoxName *zoxName, const char* text) {
    const int text_length = text != NULL ? strlen(text) : 0;
    if (zoxName->length != text_length) {
        resize_memory_component(ZoxName, zoxName, byte, text_length)
    }
    for (int i = 0; i < text_length; i++) {
        zoxName->value[i] = convert_ascii(text[i]);
    }
}

char* get_zext_text(const TextData *textData) {
    return convert_zext_to_text(textData->value, textData->length);
}

byte is_zext(TextData *zext, const char* text) {
    if (!zext || !zext->value) {
        return 0; // error
    }
    const int text_length = text != NULL ? strlen(text) : 0;
    if (zext->length != text_length) {
        return 0;
    }
    if (text_length == 0 && zext->length == 0) {
        return 1;
    }
    for (int i = 0; i < text_length; i++) {
        byte j = convert_ascii(text[i]);
        byte k = zext->value[i];
        if (j != k) {
            return 0;
        }
    }
    return 1;
}

void set_zext(TextData *data, const char* text) {
    const int text_length = text != NULL ? strlen(text) : 0;
    if (data->length != text_length) {
        resize_memory_component(TextData, data, byte, text_length);
    }
    for (int i = 0; i < text_length; i++) {
        data->value[i] = convert_ascii(text[i]);
    }
}

void print_entity_zext(ecs *world, const entity e) {
    if (!zox_has(e, TextData)) {
        return;
    }
    const TextData *data = zox_get(e, TextData)
    char *debug_text = get_zext_text(data);
    zox_log("   > zext %lu [%s] length %i\n", e, debug_text, data->length)
    free(debug_text);
}

byte is_zext_updating(ecs *world, const Children *children) {
    for (int i = 0; i < children->length; i++) { // update the reused ones
        const entity zigel = children->value[i];
        const byte generate_texture = zox_get_value(zigel, GenerateTexture)
        if (generate_texture) {
            return 1;
        }
    }
    return 0;
}


void set_entity_label_with_zext(
    ecs *world,
    const entity parent,
    byte *value,
    int length
) {
    const Children* name_label_children = zox_get(parent, Children)
    entity e = name_label_children->value[0];
    zox_muter(e, TextData, text_data);
    if (text_data->value) {
        dispose_TextData(text_data);
    }
    initialize_TextData(text_data, length);
    memcpy(text_data->value, value, length);
    text_data->length = length;
    zox_set(e, TextDirty, { zox_dirty_trigger });
}

byte set_entity_text(ecs *world, const entity e, const char* text) {
    if (!zox_valid(e) || !zox_has(e, TextData) || !zox_has(e, TextDirty)) {
        zox_log_error("invalid zext in [set_entity_text]")
        return 0;
    }
    zox_mut_begin(e, TextData, textData);
    if (!is_zext(textData, text)) {
        set_zext(textData, text);
        zox_mut_end(e, TextData);
        zox_set(e, TextDirty, { zox_dirty_trigger });
        return 1;
    } else {
        return 0;
    }
}

void set_entity_text_raw(
    ecs* world,
    const entity e,
    const char* text
) {
    TextData data = (TextData) { 0 };
    set_zext(&data, text);
    zox_set_ptr(e, TextData, data);
    zox_set(e, TextDirty, { zox_dirty_trigger });
}

byte set_entity_label_with_text(
    ecs *world,
    const entity e,
    const char* text
) {
    zox_geter(e, Children, children);
    if (children->length == 0) {
        return 0;
    }
    return set_entity_text(world, children->value[0], text);
}

void set_new_zox_name(
    ecs *world,
    const entity e,
    const char* text
) {
    zox_set_zext_component(e, ZoxName, text);
}


int get_zexts_count(ecs *world) {
    return zox_count_types(Zext);
}
