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

byte calculate_list_max_characters(const SpawnList data) {
    byte max_list_characters = 0; // get max text length out of all of the words
    for (byte i = 0; i < data.count; i++) {
        SpawnListElement child_data = data.elements[i];
        byte txt_size = child_data.text ? strlen(child_data.text) : 0;
        if (txt_size > max_list_characters) {
            max_list_characters = txt_size;
        }
    }
    return max_list_characters;
}

static inline int2 calculate_list_size(
    byte max_characters,
    SpawnList data)
{
    int list_element_height = data.font_size + data.button_padding.y * 2;
    return (int2) {
        max_characters * data.font_size +
            data.button_padding.x * 2 + data.margins.x * 2,
        data.visible_count * list_element_height +
            ((data.visible_count - 1) * data.padding.y) +
            data.margins.y * 2
    };
}

// Returns window + list
entity3 spawn_window_list(
    ecs* world,
    entity prefab,
    entity player,
    const char *header,
    byte header_font_size,
    byte list_font_size,
    ClickEvent close_event,
    byte can_close,
    byte window_type,   // TODO: Remove this
    int min_width,
    byte alignment,
    float2 position_anchor,
    byte2 padding,
    entity* elements2,
    SpawnListElement* elements,
    byte elements_count,
    byte visible_count)
{
    entity canvas = zox_get_link(world, player, Canvas);
    if (!zox_valid(canvas)) {
        zox_loge("Invalid canvas in [spawn_window_list]");
        return (entity3) { 0, 0, 0 };
    }
    // Scale all ones that are created locally
    byte2 list_margins =  (byte2) { 24 * ui_scale, 8 * ui_scale };
    byte2 header_padding = (byte2) { 10 * ui_scale, 4 * ui_scale };
    byte2 button_padding = (byte2) { 8 * ui_scale, 4 * ui_scale };
    byte slider_height = 16 * ui_scale;
    byte slider_padding = 0; // 24 * ui_scale;
    byte scrollbar_width = 10 * ui_scale;
    // # Window #
    LayoutParentData canvas_data = { .e = canvas };
    // we need to calculate header size too
    int2 header_size = calculate_header_size(
        strlen(header),
        header_font_size,
        header_padding);
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
    int2 scrollview_size = list_size;
    if (min_width) {
        if (scrollview_size.x < min_width) {
            scrollview_size.x = min_width;
        }
    }
    byte scrollbar_visible = list_data.visible_count != list_data.count;
    if (scrollbar_visible) {
        scrollview_size.x += scrollbar_width;
    }
    int2 size = (int2) {
        int_max(scrollview_size.x, header_size.x),
        list_size.y };
    // Spawn our Window
    entity3 e2 = spawn_window(
        world,
        prefab,
        prefab_body,
        header,
        canvas,
        int2_zero,
        size,
        position_anchor,
        header_font_size,
        header_padding,
        close_event.value);
    entity e = e2.x;
    entity body = e2.z;
    // NOTE: Scrollview Has: 1: Scrollbar, 2: ListUI
    entity scrollview = spawn_scrollview(
        world,
        body,
        int2_zero,
        scrollview_size,
        float2_centre,
        scrollbar_width,
        list_data.visible_count,
        list_data.count,
        scrollbar_visible);
    zox_setv(e, ScrollviewLink, scrollview);
    // Spawn our list
    ElementSpawnData list_element_data = {
        .prefab = prefab_list,
        .size = list_size,
        .anchor = float2_centre,
    };
    entity list = spawn_list(
        world,
        canvas_data,
        (LayoutParentData) {
            .e = scrollview
        },
        list_element_data,
        list_data,
        alignment,
        elements2);
    zox_setv(list, ScrollviewLink, scrollview);
    zox_setv(scrollview, ListUILink, list);
    if (scrollbar_visible) {
        zox_setv(list, LayoutPosition, ((int2) { -scrollbar_width, 0 }));
    }
    // NOTE: Returns Window, List, Header
    return (entity3) {
        e,
        list,
        e2.y
    };
}

// NOTE: This is for size position set window lists
entity4 spawn_window_list_at(
    ecs* world,
    entity prefab,
    entity canvas,
    int2 position,
    int2 size,
    byte alignment,
    float2 anchor,
    const char *header,
    byte header_font_size,
    byte list_font_size,
    byte2 padding,
    byte scrollbar_width,
    byte can_close,
    ClickEvent close_event)
{
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
    entity3 e2 = spawn_window(
        world,
        prefab,
        prefab_body,
        header,
        canvas,
        position,
        body_size,
        anchor,
        header_font_size,
        header_padding,
        close_event.value);
    entity e = e2.x;
    entity body = e2.z;
    // NOTE: Scrollview Has: 1: Scrollbar, 2: ListUI
    byte scrollbar_visible = list_data.visible_count != list_data.count;
    entity scrollview = spawn_scrollview(
        world,
        body,
        int2_zero,
        list_size,
        float2_half,
        scrollbar_width,
        list_data.visible_count,
        list_data.count,
        scrollbar_visible);
    zox_setv(e, ScrollviewLink, scrollview);
    // Spawn our list
    ElementSpawnData list_element_data = {
        .prefab = prefab_list,
        .size = list_size,
        .anchor = float2_half,
    };
    entity list = spawn_list(
        world,
        canvas_data,
        (LayoutParentData) {
            .e = scrollview
        },
        list_element_data,
        list_data,
        alignment,
        NULL);
    // make sure to link them together
    zox_set(list, ScrollviewLink, { scrollview });
    zox_set(scrollview, ListUILink, { list });
    // NOTE: Returns Window, List, Header
    return (entity4) { e, e2.y, body, list };
}
