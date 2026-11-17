// A 3D list window: a header and a column of buttons on a canvas3 panel in the world (XR's menus, zox_huds3D)
//  - the canvas3 is the Window: navigation walks its NavigationElements (the buttons), as a 2D window's
//  - parent: the player's 2D canvas, so the menu's found, closed and faded as its 2D twin is (huds look for menus
//      among the canvas's children); 2D layout and raycasts skip it (no Position2), layout3 places its children
//  - sizes in canvas pixels, as a 2D list's; scale: the panel's height in metres (layout3 spans a canvas3 one high)
//  - buttons: their text, on_click and tooltip, returned in spawned (count of them)
//  - the buttons are a list as a 2D list's (list3_list under the panel, by ChildIndex): list_system places them,
//      list_render_dirty_system shows ListVisible of them from ListStart. Over list3_rows_max buttons it scrolls,
//      navigation keeping the selection in view (scroll_list_to_child), as a 2D list's
#define list3_rows_max 8
#define list3_margin 8
#define list3_spacing 4
#define list3_button_padding_x 8
#define list3_button_padding_y 4
#define list3_header_padding 4

entity spawn_list3(
    ecs* world,
    entity parent,
    float3 position,
    float4 rotation,
    float scale,
    const char* header,
    byte header_font_size,
    byte font_size,
    const SpawnListElement* elements,
    byte count,
    entity* spawned)
{
    // the widest line sets the width
    int max_characters = 0;
    for (byte i = 0; i < count; i++) {
        const int length = elements[i].text ? strlen(elements[i].text) : 0;
        max_characters = int_max(max_characters, length);
    }
    const int header_length = header ? strlen(header) : 0;
    const int2 button_size = {
        max_characters * font_size + list3_button_padding_x * 2,
        font_size + list3_button_padding_y * 2
    };
    const byte rows = count > list3_rows_max ? list3_rows_max : count;
    const int header_height = header_length ? header_font_size + list3_header_padding * 2 : 0;
    const int2 size = {
        int_max(button_size.x, header_length * header_font_size + list3_header_padding * 2) + list3_margin * 2,
        list3_margin * 2 + header_height + (header_length ? list3_spacing : 0) +
            rows * button_size.y + (rows > 0 ? (rows - 1) * list3_spacing : 0)
    };
    entity e = spawn_canvas3(world, prefab_canvas3, position, scale, size);
    zox_set_unique_name(e, "list3");
    zox_setv(e, Rotation3D, rotation);
    zox_add(e, Window);
    if (zox_valid(parent)) {
        zox_set_parent(world, e, parent);
    }
    entity panel = spawn_uic(world, prefab_frame3, e, float2_centre, int2_zero, size, size, window_fill, window_outline);
    zox_set_unique_name(panel, "list3_panel");
    // from the top down
    int y = size.y / 2 - list3_margin;
    if (header_length) {
        const int2 header_size = { size.x - list3_margin * 2, header_height };
        entity text = spawn_text(
            world,
            prefab_text3,
            panel,
            (int2) { 0, y - header_height / 2 },
            float2_centre,
            header_font_size,
            zox_alignment_centre,
            byte2_single(list3_header_padding),
            header,
            header_font_fill,
            header_font_outline);
        zox_setv(text, LayoutSize, header_size);
        zox_setv(text, Layer, 1);
        zox_set_unique_name(text, "list3_header");
        y -= header_height + list3_spacing;
    }
    // the buttons' list, below the header
    const int list_height = rows * button_size.y + (rows > 0 ? (rows - 1) * list3_spacing : 0);
    entity list = spawn_ui(world, prefab_layout3_child, panel, float2_centre, (int2) { 0, y - list_height / 2 },
        (int2) { size.x - list3_margin * 2, list_height }, int2_zero);
    zox_set_unique_name(list, "list3_list");
    zox_setv(list, ListPadding, ((byte2) { 0, list3_spacing }));
    zox_setv(list, ListMargins, byte2_zero);
    zox_setv(list, ListAlignment, zox_alignment_centre);
    zox_setv(list, ListStart, 0);
    zox_setv(list, ListVisible, rows);
    zox_add(list, ListPositionDirty);
    for (byte i = 0; i < count; i++) {
        // placed now (list_system places them again), the rows past the shown ones hidden
        const int2 local = { 0, list_height / 2 - button_size.y / 2 - i * (button_size.y + list3_spacing) };
        entity button = spawn_uic(world, prefab_button3, list, float2_centre, local, button_size, button_size, button_fill, button_outline);
        zox_setv(button, Layer, 1);
        zox_setv(button, ChildIndex, i);
        zox_setv(button, RenderDisabled, i >= rows);
        zox_setv(button, ClickEvent, elements[i].on_click.value);
        zox_set_unique_name(button, "list3_button");
        entity text = spawn_text(
            world,
            prefab_text3,
            button,
            int2_zero,
            float2_centre,
            font_size,
            zox_alignment_centre,
            (byte2) { list3_button_padding_x, list3_button_padding_y },
            elements[i].text,
            button_font_fill,
            button_font_outline);
        zox_setv(text, LayoutSize, button_size);
        zox_setv(text, Layer, 1);
        if (spawned) {
            spawned[i] = button;
        }
    }
    return e;
}
