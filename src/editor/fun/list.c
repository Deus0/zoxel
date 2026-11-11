// Editor windows (hierarchy, inspector) are window lists: window > scrollview > list > rows
//  - when dirty, a window clears its list and spawns a button row per item

// the window's list, 0 (and logs why) if it isn't set up
entity get_editor_window_list(
    ecs* world,
    entity window)
{
    const entity scrollview = zox_get_link(world, window, ScrollviewLink);
    if (!zox_valid(scrollview)) {
        zox_loge("Editor window [%s] has no scrollview", zox_getn(window));
        return 0;
    }
    const entity list = zox_get_link(world, scrollview, ListUILink);
    if (!zox_valid(list) ||
        !zox_has(list, Layer) ||
        !zox_has(list, ListVisible) ||
        !zox_has(list, TextPadding))
    {
        zox_loge("Editor scrollview [%s] has no valid list", zox_getn(scrollview));
        return 0;
    }
    return list;
}

void clear_editor_list(
    ecs* world,
    entity list)
{
    iter it2 = zox_children(world, list);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            zox_delete(it2.entities[j]);
        }
    }
}

// spawns a row button, the first ListVisible rows show until it's scrolled
entity spawn_editor_list_row(
    ecs* world,
    entity list,
    entity prefab,
    const char* text,
    byte font_size,
    color outline,
    uint index)
{
    const byte layer = zox_getv(list, Layer) + 1;
    const byte rendered = index < zox_getv(list, ListVisible);
    entity2 e2 = spawn_button(
        world,
        prefab,
        list,
        text,
        int2_zero,
        int2_zero,
        float2_half,
        zox_alignment_centre,
        font_size,
        zox_getv(list, TextPadding),
        editor_button_fill,
        outline,
        editor_color_font,
        editor_color_fonto);
    zox_setv(e2.x, Layer, layer);
    zox_setv(e2.y, Layer, layer + 1);
    zox_setv(e2.x, RenderDisabled, !rendered);
    zox_setv(e2.y, RenderDisabled, !rendered);
    return e2.x;
}

// back to the top, then repositions and shows the new rows
void refresh_editor_list(
    ecs* world,
    entity list)
{
    zox_setv(list, ListStart, 0);
    zox_add(list, ListDirty);
    zox_add(list, ListPositionDirty);
}

// an entity's name for a row, its ZoxName if it has one
void get_editor_entity_label(
    ecs* world,
    entity e,
    char* buffer,
    uint size)
{
    if (zox_has(e, ZoxName)) {
        snprintf(buffer, size, "%s", zox_get(e, ZoxName)->value);
    } else {
        snprintf(buffer, size, "%s", zox_get_name(e));
    }
}
