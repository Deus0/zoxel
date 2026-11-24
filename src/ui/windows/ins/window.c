// Spawns a window: a header on top, its body, a footer under it for its buttons (footer_height, 0 for none, its
//  entity in footer), each a strip of its own (they don't overlap, their fills are see through)
//  - 3D huds (zox_huds3D): a canvas3 ahead of the player (ui_window3_pose), still a child of the 2D canvas (found,
//      toggled and closed as a 2D window is); its header, body and footer, and whatever the caller spawns in them,
//      are their 2D prefabs' 3D twins (ui_prefab_for), so a window's spawn code is the same for both
entity3 spawn_window_footer(
    ecs* world,
    entity prefab,
    entity prefab_body,
    const char* header,
    entity parent,
    int2 position,
    int2 body_size,
    float2 anchor,
    byte header_font_size,
    byte2 header_padding,
    void* on_closed,
    int footer_height,
    entity* footer)
{
    entity3 output;
    color fill = window_fill;
    color outline = window_outline;
    const byte is3D = zox_huds3D;
    entity e;
    if (is3D) {
        float3 position3 = float3_zero;
        float4 rotation3 = quaternion_identity;
        if (ui_window3_pose) {
            ui_window3_pose(world, parent, &position3, &rotation3);
        }
        e = spawn_canvas3(world, prefab_canvas3, position3, 1, body_size);
        zox_setv(e, Rotation3D, rotation3);
        zox_add(e, Window);
        zox_add(e, PreInitialize);  // as a 2D window's: its spawn systems run (Initialize next: a map's, a taskbar's link)
        zox_set_parent(world, e, parent);   // its close button as a 2D one's: a gamepad's B clicks it (cancel_menu_system)
    } else {
        e = zox_ins(world, prefab);    // zox_instance would declare its own e
        zox_set_parent_non_fragment(world, e, parent);
    }
    output.x = e;
    zox_set_unique_name(e, header);
    byte header_height = 0;
    // NOTE: Header Anchored to top of window
    {
        byte is_close_button = on_closed != NULL;
        color header_font_fill = header_font_fill;
        color header_font_outline = header_font_outline;
        if (header == NULL || header[0] == '\0') {
            header_font_size = 0;
        }
        header_height = header_font_size + header_padding.y * 2;
        float2 header_anchor = (float2) {
            0.5f,
            1
        };
        int2 header_size = (int2) {
            body_size.x,
            header_height
        };
        int2 header_position = (int2) {
            0,
            -header_height / 2
        };
        entity e2 = spawn_header(
            world,
            e,
            header_position,
            header_size,
            header_anchor,
            header,
            header_font_size,
            header_padding,
            is_close_button,
            on_closed);
        output.y = e2;
    }
    // NOTE: Body Anchored to Bottom of window
    {
        int2 body_position = (int2) {
            0,
            footer_height + body_size.y / 2
        };
        float2 body_anchor = (float2) {
            0.5f,
            0
        };
        entity body = spawn_uic(
            world,
            prefab_body,
            e,
            body_anchor,
            body_position,
            body_size,
            body_size,
            fill,
            outline);
        zox_set_unique_name(body, "body");
        output.z = body;
    }
    // its footer, under its body
    if (footer_height) {
        const int2 footer_size = { body_size.x, footer_height };
        const entity e2 = spawn_uic(world, prefab_element_frame, e, (float2) { 0.5f, 0 }, (int2) { 0, footer_height / 2 },
            footer_size, footer_size, footer_fill, footer_outline);
        zox_set_unique_name(e2, "footer");
        zox_add(e2, WindowFooter);
        if (footer) {
            *footer = e2;
        }
    }
    // finish setting window
    int2 size = body_size;
    size.y += header_height + footer_height;
    if (is3D) {
        // its own tooltip (a 2D canvas has one for all): its uis link to it as a 2D canvas's do (the nearest Canvas),
        //  shown under the selected one (tooltip3_follow_system)
        if (zox_valid(ui_window3_tooltip_prefab)) {
            const entity tooltip = spawn_tooltip(world, ui_window3_tooltip_prefab, e, 0, 0);
            zox_add(tooltip, DisableParentTransform);
            // centred where it's put (a 2D one's top right corner sits at the mouse)
            zox_setv(tooltip, MeshAlignment, zox_alignment_centre);
            zox_setv(tooltip, TextAlignment, zox_alignment_centre);
        }
        zox_setv(e, LayoutSize, size);
        zox_setv(e, Scale1, size.y * ui_window3_metres_per_pixel / (ui_scale > 0 ? ui_scale : 1));
        return output;
    }
    // NOTE: [0,1] => [-0.5, +0.5]
    float2 anchor_shift = float2_sub(anchor, float2_half);
    position.x -= anchor_shift.x * size.x;
    position.y -= anchor_shift.y * size.y;
    zox_setv(e, LayoutPosition, position);
    zox_setv(e, LayoutSize, size);
    zox_setv(e, Anchor, anchor);
    // TODO: Fix WindowToTop by just checking reset WindowLayers when CanvasDirty is flagged - Flag CanvasDirty when new window Initializes
    zox_setv(parent, WindowToTop, e);
    // every window pops in (ui_pop_in), and out when closed (ui_pop_out, toggles, close buttons)
    ui_pop_in(world, e);
    return output;
}

// a window without a footer
entity3 spawn_window(ecs* world, entity prefab, entity prefab_body, const char* header, entity parent, int2 position,
    int2 body_size, float2 anchor, byte header_font_size, byte2 header_padding, void* on_closed)
{
    return spawn_window_footer(world, prefab, prefab_body, header, parent, position, body_size, anchor, header_font_size,
        header_padding, on_closed, 0, NULL);
}

// OBSOLETE REMOVE
entity2 spawn_window_old(
    ecs *world,
    LayoutParentData canvas_data,
    LayoutParentData parent_data,
    ElementSpawnData element_data,
    SpawnWindow2 window_data,
    ClickEvent on_click,
    byte is_close_button,
    byte type)
{
    byte header_height = window_data.header_font_size + window_data.header_padding.y * 2;
    byte header_font_thickness_s = header_font_thickness * ui_scale;
    byte header_fonto_thickness_s = header_font_thickness * ui_scale;
    zox_instance(element_data.prefab);
    zox_name("window");
    set_element_spawn_data(world, e, canvas_data, parent_data, element_data);
    LayoutParentData e_parent_data = { .e = e };
    entity header;
    {
        // # Window Header #
        // todo: pass more of t this in from top
        ElementSpawnData header_element_data = {
            .prefab = prefab_header,
            .anchor = (float2) { 0.5f, 1.0f },
            .position = (int2) { 0, - header_height / 2 },
            .size = (int2) { element_data.size.x, header_height },
            .layer = element_data.layer + 1,
        };
        SpawnHeaderData header_data = {
            .prefab_text = prefab_text,
            .is_close_button = is_close_button
        };
        SpawnTextData header_text_data = {
            .text = window_data.header_text,
            .font_size = window_data.header_font_size,
            .font_resolution = window_data.header_font_size, // header_font_resolution,
            .font_thickness = header_font_thickness_s,
            .font_outline_thickness = header_fonto_thickness_s,
            .font_fill_color = header_font_fill,
            .font_outline_color = header_font_outline,
            .margins = window_data.header_padding,
        };
        header = spawn_header_old(
            world,
            canvas_data,
            e_parent_data,
            header_element_data,
            header_text_data,
            header_data,
            on_click);
        zox_set_parent_non_fragment(world, header, e);
    }
    return (entity2) { e, header };
}
