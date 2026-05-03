entity spawn_label_background(ecs *world, entity prefab, entity parent, entity canvas, int2 pixel_position, float2 anchor, byte2 padding, const char* text, byte font_size, byte alignment, byte layer, int2 parent_position, int2 parent_size, color fill, color fillo, color font_fill, color font_fillo, byte render_disabled) {

    if (!zox_valid(canvas)) {
        zox_logw("Canvas invalid in [spawn_label_background]");
        return 0;
    }

    SpawnZext zextSpawnData = {
        .canvas = {
            .e = canvas,
        },
        .parent = {
            .e = parent,
            .position = parent_position,
        },
        .element = {
            .prefab = prefab,
            .layer = layer,
            .anchor = anchor,
            .position = pixel_position,
            .render_disabled = render_disabled,
        },
        .zext = {
            .text = text,
            .font_resolution = font_size, // 32,
            .font_size = font_size,
            .font_thickness = 1,
            .font_outline_thickness = 1,
            .alignment = alignment,
            .margins = padding,
            .font_fill_color = font_fill,
            .font_outline_color = font_fillo
        }
    };

    entity e = spawn_zext(world, zextSpawnData);

    if (!zox_valid(e)) {
        zox_loge("Invalid e in [spawn_label_background]");
        return 0;
    }

    zox_name("label_background");

    zox_set(e, Color, { fill });
    zox_set(e, OutlineColor, { fillo });

    return e;
}
