// Colors of the generated + texture
const color stat_plus_fill = { 125, 255, 125, 255 };
const color stat_plus_outline = { 15, 55, 15, 255 };

// The users currency spent on attributes
entity get_user_stat_point(
    ecs* world,
    entity user)
{
    if (!zox_valid(user)) {
        return 0;
    }
    return zox_get_child_by_id(world, user, zox_id(StatPoint));
}

// Spends a stat point on the buttons attribute
void on_click_stat_plus(
    ecs* world,
    ClickEventData data)
{
    byte dbg_log = 0;
    entity button = data.clicked;
    if (!zox_valid(button) ||
        !zox_is_linked_any(world, button, DataLink))
    {
        return;
    }
    entity attribute = zox_get_link(world, button, DataLink);
    if (!zox_valid(attribute) ||
        !zox_has(attribute, StatAttribute))
    {
        return;
    }
    entity user = zox_get_parent(world, attribute);
    entity points = get_user_stat_point(world, user);
    if (!zox_valid(points)) {
        return;
    }
    StatValue* points_value = zox_mut(points, StatValue);
    if (points_value->value < 1) {
        return;
    }
    StatValue* attribute_value = zox_mut(attribute, StatValue);
    points_value->value -= 1;
    attribute_value->value += 1;
    zox_add(points, Dirty);
    zox_add(points, DataDirty);
    zox_add(attribute, Dirty);
    zox_add(attribute, DataDirty);
    if (dbg_log) {
        zox_log("[%s] spent stat point on [%s] -> [%i], points left [%i]",
            zox_getn(user),
            zox_getn(attribute),
            (int) attribute_value->value,
            (int) points_value->value);
    }
}

// Shows a + button on attribute frames while the user has a stat point
// NOTE: system (not observer) so a frame never gets two buttons in one frame
void stat_plus_button_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity data = zox_get_link(world, e, DataLink);
        entity stat = data;
        if (!zox_valid(stat) ||
            !zox_has(stat, StatAttribute))
        {
            continue;
        }
        entity frame = zox_get_parent(world, e);
        if (!zox_valid(frame)) {
            continue;
        }
        entity user = zox_get_parent(world, stat);
        entity points = get_user_stat_point(world, user);
        byte has_points =
            zox_valid(points) &&
            zox_getv(points, StatValue) >= 1;
        entity button = zox_get_child_by_id(world, frame, zox_id(StatPlusButton));
        if (has_points && !zox_valid(button)) {
            const int2 frame_size = zox_getv(frame, LayoutSize);
            const int2 size = int2_single(frame_size.y / 3);
            // NOTE: spawned after window layers set, so set them here too
            const byte frame_layer = zox_has(frame, Layer) ? zox_getv(frame, Layer) : 0;
            // Button background, inside top left corner of frame
            // NOTE: anchor is the buttons center, so shift in by half its size (y is up)
            const int margin = ui_scale;
            const int2 position = (int2) {
                size.x / 2 + margin,
                -(size.y / 2 + margin)
            };
            button = spawn_uic(
                world,
                prefab_button,
                frame,
                float2_top_left,
                position,
                size,
                size,
                button_fill,
                button_outline);
            zox_set_unique_name(button, "stat_plus_button");
            zox_setv(button, ElementFillColor, button_fill);
            zox_setv(button, ElementOutlineColor, button_outline);
            zox_add(button, StatPlusButton);
            zox_set_link(world, button, DataLink, stat);
            zox_setv(button, ClickEvent, &on_click_stat_plus);
            // above the frames icon and label (frame + 1)
            zox_setv(button, BonusLayer, 1);
            zox_setv(button, Layer, frame_layer + 2);
            // Generated + texture on top
            const int2 plus_size = int2_single((size.y * 3) / 4);
            const byte plus_thickness = plus_size.y >= 16 ? 2 : 1;
            entity plus = spawn_uic(
                world,
                prefab_element,
                button,
                float2_half,
                int2_zero,
                plus_size,
                plus_size,
                stat_plus_fill,
                stat_plus_outline);
            zox_set_unique_name(plus, "stat_plus_icon");
            zox_add(plus, PlusTexture);
            zox_setv(plus, OutlineThickness, plus_thickness);
            zox_setv(plus, Layer, frame_layer + 3);
            zox_setv(plus, Generate, zox_dirty_trigger);
            if (dbg_log) {
                zox_log("+ Stat Plus Button [%s]", zox_getn(stat));
            }
        } else if (!has_points && zox_valid(button)) {
            zox_delete(button);
            if (dbg_log) {
                zox_log("- Stat Plus Button [%s]", zox_getn(stat));
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(stat_plus_button_system);
