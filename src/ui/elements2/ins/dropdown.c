static void dropdown_close(ecs* world, entity dropdown) {
    const entity popup = zox_get_child_by_id(world, dropdown, zox_id(DropdownPopup));
    if (popup) {
        zox_delete(popup);
    }
}

static void dropdown_select(ecs* world, ClickEventData event) {
    const entity popup = zox_get_parent(world, event.clicked);
    const entity dropdown = zox_get_parent(world, popup);
    const entity label = zox_get_child_by_id(world, event.clicked, zox_id(TextData));
    const entity selected = zox_get_child_by_id(world, dropdown, zox_id(TextLabel));
    char* text = get_zext_text(zox_get(label, TextData));
    set_entity_text(world, selected, text ? text : "");
    free(text);
    zox_setv(dropdown, DropdownIndex, zox_getv(event.clicked, ChildIndex));
    dropdown_close(world, dropdown);
    zox_add(dropdown, Dirty);
}

static void dropdown_toggle(ecs* world, ClickEventData event) {
    const entity dropdown = event.clicked;
    if (zox_get_child_by_id(world, dropdown, zox_id(DropdownPopup))) {
        dropdown_close(world, dropdown);
        return;
    }
    const DropdownOptions* options = zox_get(dropdown, DropdownOptions);
    const char* text = options->value;
    if (!text) {
        return;
    }
    const entity canvas = zox_get_link(world, dropdown, DropdownCanvas);
    const entity previous = zox_get_link(world, canvas, OpenDropdown);
    if (zox_valid(previous) && previous != dropdown) {
        dropdown_close(world, previous);
    }
    const int2 size = zox_getv(dropdown, LayoutSize);
    const byte font = zox_getv(dropdown, ElementFontSize);
    const byte overlay_layer = 240;
    SpawnListElement rows[layouts2_children_capacity];
    byte count = 0;
    while (text < options->value + options->length) {
        rows[count++] = (SpawnListElement) { .text = text, .on_click = { dropdown_select } };
        text += strlen(text) + 1;
    }
    entity buttons[count];
    const int height = count * size.y;
    const entity popup = spawn_list(world, (LayoutParentData) { .e = canvas }, (LayoutParentData) { .e = dropdown },
        (ElementSpawnData) { .prefab = prefab_list, .anchor = { 0.5f, 0 }, .position = { 0, -height / 2 },
            .size = { size.x, height }, .layer = overlay_layer },
        (SpawnList) { .count = count, .visible_count = count, .elements = rows, .font_size = font,
            .button_padding = { 2, (size.y - font) / 2 } }, zox_alignment_centre, buttons);
    zox_add(popup, DropdownPopup);
    for (int i = 0; i < count; i++) {
        zox_setv(buttons[i], LayoutSize, size);
        if (!ui_is3D(world, dropdown)) {
            zox_setv(buttons[i], Layer, overlay_layer + 1);
        }
    }
    zox_set_link(world, canvas, OpenDropdown, dropdown);
}

entity spawn_dropdown(ecs* world, entity parent, entity canvas, int2 position, int2 size, float2 anchor,
    const char* const* options, byte count, byte selected, byte font_size, byte layer)
{
    count = int_min(count, int_min(layouts2_children_capacity, 255));
    selected = selected < count ? selected : 0;
    const entity2 button = spawn_button(world, prefab_dropdown, parent, count ? options[selected] : "", position,
        size, anchor, zox_alignment_centre, font_size, byte2_single(0),
        button_fill, button_outline, button_font_fill, button_font_outline);
    zox_add(button.y, TextLabel);
    zox_setv(button.x, ClickEvent, dropdown_toggle);
    zox_setv(button.x, DropdownIndex, selected);
    zox_setv(button.x, ElementFontSize, font_size);
    zox_set_link(world, button.x, DropdownCanvas, canvas);
    const entity arrow = spawn_text(world, prefab_text, button.x, (int2) { -font_size / 2 - 2, 0 },
        (float2) { 1, 0.5f }, font_size, zox_alignment_centre, byte2_single(0), "v",
        button_font_fill, button_font_outline);
    if (!ui_is3D(world, button.x)) {
        zox_setv(button.x, Layer, layer);
        zox_setv(button.y, Layer, layer + 1);
        zox_setv(arrow, Layer, layer + 1);
    }
    DropdownOptions data = { 0 };
    int length = 0;
    for (int i = 0; i < count; i++) {
        length += strlen(options[i]) + 1;
    }
    initialize_DropdownOptions(&data, length);
    char* out = data.value;
    for (int i = 0; out && i < count; i++) {
        const size_t size = strlen(options[i]) + 1;
        memcpy(out, options[i], size);
        out += size;
    }
    zox_set_ptr(button.x, DropdownOptions, data);
    dispose_DropdownOptions(&data);
    return button.x;
}
