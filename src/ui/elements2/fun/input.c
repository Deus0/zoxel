// input fields: a button to click and type in (TextInput), its value its label's text
//  - clicking one focuses it: text typed (zox_text_typed) goes in, backspace takes a character off, enter or escape
//    unfocuses it, a '_' after its text shows the focus
//  - input_field_value gives its text (without the '_'), input_field_set_value sets it
#define input_field_max_length 24
entity input_field_focus = 0;

static inline entity input_field_label(ecs* world, entity field) {
    return zox_valid(field) ? zox_get_child_by_id_recursive(world, field, zox_id(Text)) : 0;
}

// its text, without the focus '_' (free it)
char* input_field_value(ecs* world, entity field) {
    const entity label = input_field_label(world, field);
    char* text = zox_valid(label) ? get_zext_text(zox_get(label, TextData)) : NULL;
    if (!text) {
        text = malloc(1);
        text[0] = 0;
    }
    const int length = strlen(text);
    if (field == input_field_focus && length && text[length - 1] == '_') {
        text[length - 1] = 0;
    }
    return text;
}

void input_field_set_value(ecs* world, entity field, const char* value) {
    const entity label = input_field_label(world, field);
    if (!zox_valid(label)) {
        return;
    }
    char text[input_field_max_length + 2];
    snprintf(text, sizeof(text), "%.*s%s", input_field_max_length, value, field == input_field_focus ? "_" : "");
    if (set_entity_text(world, label, text)) {
        zox_add(field, Dirty);
    }
}

// re-sets its text, with or without the '_'
static inline void input_field_refresh(ecs* world, entity field, entity focus) {
    char* value = input_field_value(world, field);
    input_field_focus = focus;
    input_field_set_value(world, field, value);
    free(value);
}

void input_field_unfocus(ecs* world) {
    const entity old = input_field_focus;
    zox_text_input_wanted = 0;
    if (zox_valid(old)) {
        input_field_refresh(world, old, 0);
    }
    input_field_focus = 0;
}

void input_field_set_focus(ecs* world, entity field) {
    if (field == input_field_focus) {
        return;
    }
    input_field_unfocus(world);
    if (!zox_valid(field)) {
        return;
    }
    input_field_refresh(world, field, field);
    zox_text_input_wanted = 1;
}

// adds typed text to the focused field
void input_field_type(ecs* world, const char* typed) {
    if (!zox_valid(input_field_focus) || !typed[0]) {
        return;
    }
    char* value = input_field_value(world, input_field_focus);
    char text[input_field_max_length + 1];
    snprintf(text, sizeof(text), "%s%s", value, typed);
    free(value);
    input_field_set_value(world, input_field_focus, text);
}

void input_field_backspace(ecs* world) {
    if (!zox_valid(input_field_focus)) {
        return;
    }
    char* value = input_field_value(world, input_field_focus);
    const int length = strlen(value);
    if (length) {
        value[length - 1] = 0;
    }
    input_field_set_value(world, input_field_focus, value);
    free(value);
}

void on_click_input_field(ecs* world, ClickEventData event) {
    input_field_set_focus(world, event.clicked);
}
