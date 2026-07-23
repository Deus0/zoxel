// List Menus adjust to the menu size
// TODO: Remove all structs
typedef struct {
    entity player;
    const char *header;
    byte header_font_size;
    SpawnListElement* elements;
    byte elements_count;
    byte visible_count;
    byte list_font_size;
    ClickEvent close_event;
    byte can_close;
} WindowListSpawnData;

// Returns window + list
entity3 spawn_window_list(ecs* world, entity prefab, entity player, const char *header, byte header_font_size, byte list_font_size, ClickEvent close_event, byte can_close, byte window_type, int min_width, byte alignment, float2 position_anchor, byte2 padding, entity* elements2, SpawnListElement* elements, byte elements_count, byte visible_count) {
    entity canvas = zox_getv(player, CanvasLink);
    if (!zox_valid(canvas)) {
        zox_loge("Invalid canvas in [spawn_window_list]");
        return (entity3) { 0, 0, 0 };
    }
    // Scale all ones that are created locally
    // byte header_font_size = 8 * ui_scale;
    byte2 header_padding = (byte2) { 10 * ui_scale, 4 * ui_scale };
    byte2 button_padding = (byte2) { 8 * ui_scale, 4 * ui_scale };
    byte2 list_margins =  (byte2) { 16 * ui_scale, 8 * ui_scale };
    byte slider_height = 16 * ui_scale;
    byte slider_padding = 24 * ui_scale;
    // # Window #
    LayoutParentData canvas_data = { .e = canvas };
    // we need to calculate header size too
    int2 header_size = calculate_header_size(strlen(header), header_font_size, header_padding);
    // # List #
    SpawnList list_data = (SpawnList) {
        .visible_count = visible_count,
        .count = elements_count,
        .elements = elements,
        .font_size = list_font_size,
        .fill = window_fill,
        .outline = window_outline,
        .button_padding = button_padding,
        .padding = padding,
        .margins = list_margins,
        .slider_height = slider_height,
        .slider_padding = slider_padding,
    };
    // Our window again, spawn using list size
    // calculate size
    int2 list_size = calculate_list_size(
        calculate_list_max_characters(list_data),
        list_data
    );
    if (min_width) {
        if (list_size.x < min_width) {
            // zox_log("+ list size [%i] < min_width [%i]", list_size.x, min_width);
            list_size.x = min_width;
        }
    }
    // zox_log("+ list size [%ix%i] from visible [%i] header_height [%i]", list_size.x, list_size.y, visible_count, header_height);
    // we use the bigger size out of list and header widths
    int2 size = (int2) { int_max(list_size.x, header_size.x), list_size.y };
    // Spawn our Window
    entity3 e2 = spawn_window(world, prefab, prefab_body, header, canvas, int2_zero, size, position_anchor, header_font_size, header_padding, close_event.value);
    entity e = e2.x;
    entity body = e2.z;
    // NOTE: Scrollview Has: 1: Scrollbar, 2: ListUI
    entity scrollview = spawn_scrollview(world, body, int2_zero, size, float2_half, 8, list_data.visible_count, list_data.count);
    zox_set(e, ScrollviewLink, { scrollview });
    // Spawn our list
    ElementSpawnData list_element_data = {
        .prefab = prefab_list,
        .size = list_size,
        .anchor = float2_half,
    };
    entity list = spawn_list(world, canvas_data, (LayoutParentData) { .e = scrollview }, list_element_data, list_data, alignment, elements2);
    // make sure to link them together
    zox_set(list, ScrollviewLink, { scrollview });
    zox_set(scrollview, ListUILink, { list });
    // NOTE: Returns Window, List, Header
    return (entity3) { e, list, e2.y };
}

// NOTE: This is for size position set window lists
entity4 spawn_window_list_at(ecs* world, entity prefab, entity canvas, int2 position, int2 size, byte alignment, float2 anchor, const char *header, byte header_font_size, byte list_font_size, byte2 padding, byte scrollbar_width, byte can_close, ClickEvent close_event) {
    // Scale all ones that are created locally
    byte2 header_padding = (byte2) { 10 * ui_scale, 4 * ui_scale };
    int2 header_size = calculate_header_size(strlen(header), header_font_size, header_padding);
    byte2 button_padding = (byte2) { 8 * ui_scale, 4 * ui_scale };
    byte2 list_margins =  (byte2) { 16 * ui_scale, 8 * ui_scale };
    // # Window #
    LayoutParentData canvas_data = { .e = canvas };
    // # List #
    SpawnList list_data = (SpawnList) {
        .font_size = list_font_size,
        .fill = window_fill,
        .outline = window_outline,
        .button_padding = button_padding,
        .padding = padding,
        .margins = list_margins,
    };
    int2 list_size = (int2) { size.x, size.y - header_size.y };
    int2 body_size = list_size;
    // NOTE: Calculates visible count from size itself
    list_data.visible_count = (list_size.y - list_margins.y * 2) / (list_font_size + button_padding.y * 2 + padding.y);
    // Spawn our Window
    entity3 e2 = spawn_window(world, prefab, prefab_body, header, canvas, position, body_size, anchor, header_font_size, header_padding, close_event.value);
    entity e = e2.x;
    entity body = e2.z;
    // NOTE: Scrollview Has: 1: Scrollbar, 2: ListUI
    entity scrollview = spawn_scrollview(world, body, int2_zero, list_size, float2_half, scrollbar_width, list_data.visible_count, list_data.count);
    zox_set(e, ScrollviewLink, { scrollview });
    // Spawn our list
    ElementSpawnData list_element_data = {
        .prefab = prefab_list,
        .size = list_size,
        .anchor = float2_half,
    };
    entity list = spawn_list(world, canvas_data, (LayoutParentData) { .e = scrollview }, list_element_data, list_data, alignment, NULL);
    // make sure to link them together
    zox_set(list, ScrollviewLink, { scrollview });
    zox_set(scrollview, ListUILink, { list });
    // NOTE: Returns Window, List, Header
    return (entity4) { e, e2.y, body, list };
}
