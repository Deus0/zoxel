static inline const int4* get_ui_clip(ecs* world, entity e) {
    const LayoutClip* clip = zox_get(e, LayoutClip);
    return clip ? &clip->value : NULL;
}

static inline int4 intersect_layout_clips(int4 a, int4 b) {
    return (int4) { int_max(a.x, b.x), int_min(a.y, b.y), int_max(a.z, b.z), int_min(a.w, b.w) };
}

static int4 mask_rectangle(ecs* world, entity e, int2 canvas_size) {
    const float2 position = zox_getv(e, Position2);
    const int2 size = zox_getv(e, LayoutSize);
    const float2 scale = zox_has(e, Scale2) ? zox_getv(e, Scale2) :
        (zox_has(e, Scale1) ? float2_single(zox_getv(e, Scale1)) : float2_one);
    const float rotation = zox_has(e, Rotation2) ? zox_getv(e, Rotation2) : 0;
    const float w = fabsf(size.x * scale.x) * 0.5f;
    const float h = fabsf(size.y * scale.y) * 0.5f;
    const float half_width = fabsf(cosf(rotation)) * w + fabsf(sinf(rotation)) * h;
    const float half_height = fabsf(sinf(rotation)) * w + fabsf(cosf(rotation)) * h;
    const float x = canvas_size.x * 0.5f + position.x * canvas_size.y;
    const float y = (position.y + 0.5f) * canvas_size.y;
    return (int4) { roundf(x - half_width), roundf(x + half_width),
        roundf(y - half_height), roundf(y + half_height) };
}

static void update_mask_subtree(ecs* world, entity e, int2 canvas_size, const int4* inherited, byte apply_mask) {
    const int4* previous = get_ui_clip(world, e);
    if (inherited && (!previous || !int4_equals(*previous, *inherited))) {
        zox_setv(e, LayoutClip, *inherited);
    } else if (!inherited && previous) {
        zox_remove(e, LayoutClip);
    }
    int4 bounds;
    if (canvas_size.y > 0 && apply_mask && zox_has(e, Mask) && zox_has(e, Position2) && zox_has(e, LayoutSize)) {
        bounds = mask_rectangle(world, e, canvas_size);
        if (inherited) {
            bounds = intersect_layout_clips(*inherited, bounds);
        }
        inherited = &bounds;
    }
    iter children = zox_children(world, e);
    while (zox_children_next(children)) {
        for (int i = 0; i < children.count; i++) {
            update_mask_subtree(world, children.entities[i], canvas_size, inherited, 1);
        }
    }
}
