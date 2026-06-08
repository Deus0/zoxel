// List Menus adjust to the menu size

// TODO: Pass in struct data

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
entity3 spawn_window_list(ecs *world, entity prefab, entity player, const char *header, byte header_font_size, byte list_font_size, ClickEvent close_event, byte can_close, byte window_type, int min_width, byte alignment, byte2 padding, entity* elements2, SpawnListElement* elements, byte elements_count, byte visible_count) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    if (!zox_valid(canvas)) {
        zox_logw("Invalid canvas in [spawn_window_list]");
        return (entity3) { 0, 0, 0 };
    }
    // Sizing
    byte2 list_padding = (byte2) { padding.x, padding.y  };
    // Scale all ones that are created locally
    byte2 header_padding = (byte2) { 6 * ui_scale, 2 * ui_scale };
    byte2 button_padding = (byte2) { 8 * ui_scale, 4 * ui_scale };
    byte2 list_margins =  (byte2) { 16 * ui_scale, 8 * ui_scale };
    byte slider_height = 16 * ui_scale;
    byte slider_padding = 24 * ui_scale;
    byte window_layer = 3;    // does tihs matter? should get sorted after anyway?
    // # Window #
    // int2 canvas_size = zox_gett_value(canvas, LayoutSize);
    LayoutParentData canvas_data = {
        .e = canvas,
        // .size = canvas_size  // need for bounds
    };
    // we need to calculate header size too
    int2 header_size = calculate_header_size(strlen(header), header_font_size, header_padding);
    int header_height = header_size.y;
    // # List #
    SpawnList list_data = (SpawnList) {
        .visible_count = visible_count,
        .count = elements_count,
        .elements = elements,
        .font_size = list_font_size,
        .fill = window_fill,
        .outline = window_outline,
        .button_padding = button_padding,
        .padding = list_padding,
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
    int2 size = (int2) { int_max(list_size.x, header_size.x), list_size.y }; // + header_height };
    // Spawn our Window
    // entity2 e2 = spawn_window_old(world, canvas_data, (LayoutParentData) { .e = canvas }, window_element_data, window_data, close_event, can_close, window_type);
    entity3 e2 = spawn_window(world, prefab, prefab_body, header, canvas, int2_zero, size, float2_half, close_event.value);
    entity e = e2.x;
    entity body = e2.z;
    // NOTE: Scrollview Has: 1: Scrollbar, 2: ListUI
    entity scrollview = spawn_scrollview(world, body, int2_zero, size, float2_half, list_data.visible_count, list_data.count);
    zox_set(e, ScrollviewLink, { scrollview });
    // zox_set_parent(world, scrollview, e);
    // Spawn our list
    ElementSpawnData list_element_data = {
        .prefab = prefab_list,
        .size = list_size,
        .anchor = float2_half,
    };
    entity list = spawn_list(world, canvas_data, (LayoutParentData) { .e = scrollview }, list_element_data, list_data, alignment, elements2);
    // zox_set_parent(world, list, scrollview);
    // make sure to link them together
    zox_set(list, ScrollviewLink, { scrollview });
    zox_set(scrollview, ListUILink, { list });
    zox_muter(player, ElementLinks, pelements);
    add_to_ElementLinks(pelements, e);
    zox_set(e, ElementHolder, { player });
    return (entity3) { e, list, e2.y };
}
