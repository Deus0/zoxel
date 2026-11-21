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

// a window list's sizes on a canvas: its list, scrollview and body (the window's, without its header), the header's
//  font and the list's (in list_data) shrunk to fit the canvas, its visible count too
static inline void window_list_fit(ecs* world, entity canvas, const char* header, byte* header_font_size,
    byte2 header_padding, SpawnList* list_data, int min_width, byte scrollbar_width, int2* list_size,
    int2* scrollview_size, byte* scrollbar_visible, int2* size)
{
    int2 header_size = calculate_header_size(strlen(header), *header_font_size, header_padding);
    *list_size = calculate_list_size(calculate_list_max_characters(*list_data), *list_data);
    *scrollview_size = *list_size;
    if (min_width && scrollview_size->x < min_width) {
        scrollview_size->x = min_width;
    }
    *scrollbar_visible = list_data->visible_count != list_data->count;
    if (*scrollbar_visible) {
        scrollview_size->x += scrollbar_width;
    }
    *size = (int2) { int_max(scrollview_size->x, header_size.x), list_size->y };
    // a window is never wider than its canvas, its fonts shrink to fit instead
    //  - ui_scale follows the screen's area, so narrow screens (phones in portrait) overflowed
    const int max_width = zox_getv(canvas, LayoutSize).x - 2 * (int) (4 * ui_scale);
    if (max_width > 0 && size->x > max_width) {
        const byte max_characters = calculate_list_max_characters(*list_data);
        // the list's width without its text (paddings, margins, scrollbar)
        const int list_fixed = list_size->x - max_characters * list_data->font_size +
            (*scrollbar_visible ? scrollbar_width : 0);
        if (max_characters && list_size->x + (*scrollbar_visible ? scrollbar_width : 0) > max_width) {
            list_data->font_size = (byte) int_max(4, (max_width - list_fixed) / max_characters);
        }
        const int header_characters = strlen(header);
        if (header_characters && header_size.x > max_width) {
            *header_font_size = (byte) int_max(4, (max_width - header_padding.x * 2) / header_characters);
            header_size = calculate_header_size(header_characters, *header_font_size, header_padding);
        }
        *list_size = calculate_list_size(max_characters, *list_data);
        *scrollview_size = *list_size;
        if (min_width && scrollview_size->x < min_width) {
            scrollview_size->x = min_width;
        }
        if (*scrollbar_visible) {
            scrollview_size->x += scrollbar_width;
        }
        *size = (int2) { int_max(scrollview_size->x, header_size.x), list_size->y };
    }
    // and never taller: shows fewer elements at once (the list scrolls), at least one
    const int max_height = zox_getv(canvas, LayoutSize).y - 2 * (int) (4 * ui_scale);
    if (max_height > 0 && size->y + header_size.y > max_height && list_data->visible_count > 1) {
        const int element_height = list_data->font_size + list_data->button_padding.y * 2;
        const int list_space = max_height - header_size.y - list_data->margins.y * 2 + list_data->padding.y;
        const int fit = int_max(1, list_space / (element_height + list_data->padding.y));
        if (fit < list_data->visible_count) {
            list_data->visible_count = fit;
            *scrollbar_visible = list_data->visible_count != list_data->count;
            *list_size = calculate_list_size(calculate_list_max_characters(*list_data), *list_data);
            *scrollview_size = *list_size;
            if (min_width && scrollview_size->x < min_width) {
                scrollview_size->x = min_width;
            }
            if (*scrollbar_visible) {
                scrollview_size->x += scrollbar_width;
            }
            *size = (int2) { int_max(scrollview_size->x, header_size.x), list_size->y };
        }
    }
}

// a window list's list data: its elements and fonts, the paddings and margins every one has
static inline SpawnList window_list_data(SpawnListElement* elements, byte count, byte visible_count, byte font_size,
    byte2 padding)
{
    return (SpawnList) {
        .visible_count = visible_count,
        .count = count,
        .elements = elements,
        .font_size = font_size,
        .fill = window_fill,
        .outline = window_outline,
        .button_padding = (byte2) { 8 * ui_scale, 4 * ui_scale },
        .padding = padding,
        .margins = (byte2) { 24 * ui_scale, 8 * ui_scale },
        .slider_height = 16 * ui_scale,
        .slider_padding = 0,
    };
}

// a scrollview and its list in a window's body (at position in it), linked both ways; returns the scrollview, the list
static inline entity2 spawn_window_list_view(ecs* world, entity canvas, entity body, int2 position,
    SpawnList list_data, int2 list_size, int2 scrollview_size, byte scrollbar_visible, byte scrollbar_width,
    byte alignment, entity* elements2)
{
    // NOTE: Scrollview Has: 1: Scrollbar, 2: ListUI
    const entity scrollview = spawn_scrollview(world, body, position, scrollview_size, float2_centre, scrollbar_width,
        list_data.visible_count, list_data.count, scrollbar_visible);
    ElementSpawnData list_element_data = {
        .prefab = prefab_list,
        .size = list_size,
        .anchor = float2_centre,
    };
    const entity list = spawn_list(world, (LayoutParentData) { .e = canvas }, (LayoutParentData) { .e = scrollview },
        list_element_data, list_data, alignment, elements2);
    zox_set_link(world, list, ScrollviewLink, scrollview);
    zox_set_link(world, scrollview, ListUILink, list);
    if (scrollbar_visible) {
        zox_setv(list, LayoutPosition, ((int2) { -scrollbar_width, 0 }));
    }
    return (entity2) { scrollview, list };
}

// Returns window + list
// on a canvas (spawn_window_list: its player's)
entity3 spawn_window_list_on(
    ecs* world,
    entity prefab,
    entity canvas,
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
    if (!zox_valid(canvas)) {
        zox_loge("Invalid canvas in [spawn_window_list]");
        return (entity3) { 0, 0, 0 };
    }
    const byte2 header_padding = (byte2) { 10 * ui_scale, 4 * ui_scale };
    const byte scrollbar_width = 10 * ui_scale;
    SpawnList list_data = window_list_data(elements, elements_count, visible_count, list_font_size, padding);
    int2 list_size, scrollview_size, size;
    byte scrollbar_visible;
    window_list_fit(world, canvas, header, &header_font_size, header_padding, &list_data, min_width, scrollbar_width,
        &list_size, &scrollview_size, &scrollbar_visible, &size);
    const entity3 e2 = spawn_window(world, prefab, prefab_body, header, canvas, int2_zero, size, position_anchor,
        header_font_size, header_padding, close_event.value);
    const entity e = e2.x;
    const entity2 view = spawn_window_list_view(world, canvas, e2.z, int2_zero, list_data, list_size, scrollview_size,
        scrollbar_visible, scrollbar_width, alignment, elements2);
    zox_set_link(world, e, ScrollviewLink, view.x);
    // NOTE: Returns Window, List, Header
    return (entity3) { e, view.y, e2.y };
}

// on its player's canvas
entity3 spawn_window_list(ecs* world, entity prefab, entity player, const char *header, byte header_font_size,
    byte list_font_size, ClickEvent close_event, byte can_close, byte window_type, int min_width, byte alignment,
    float2 position_anchor, byte2 padding, entity* elements2, SpawnListElement* elements, byte elements_count,
    byte visible_count)
{
    return spawn_window_list_on(world, prefab, zox_get_link(world, player, CanvasLink), header, header_font_size,
        list_font_size, close_event, can_close, window_type, min_width, alignment, position_anchor, padding, elements2,
        elements, elements_count, visible_count);
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
    zox_set_link(world, e, ScrollviewLink, scrollview);
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
    zox_set_link(world, list, ScrollviewLink, scrollview);
    zox_set_link(world, scrollview, ListUILink, list);
    // NOTE: Returns Window, List, Header
    return (entity4) { e, e2.y, body, list };
}
