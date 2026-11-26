// Position2 is modified by the transform pass after layout. Size changes can also
// affect a mask whose centre stays fixed.
static void mask_layout_changed(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_add(it->entities[i], Dirty);
    }
}

static void update_masks(ecs* world, entity e, byte apply_mask) {
    const entity canvas = zox_has(e, Canvas) ? e : zox_get_parent_by_id(world, e, zox_id(Canvas));
    const int2 canvas_size = canvas ? zox_getv(canvas, LayoutSize) : int2_zero;
    int4 inherited;
    const int4* clip = NULL;
    for (entity parent = zox_get_parent(world, e); canvas_size.y > 0 && zox_valid(parent);
        parent = zox_get_parent(world, parent)) {
        if (zox_has(parent, Mask) && zox_has(parent, Position2) && zox_has(parent, LayoutSize)) {
            const int4 bounds = mask_rectangle(world, parent, canvas_size);
            inherited = clip ? intersect_layout_clips(inherited, bounds) : bounds;
            clip = &inherited;
        }
        if (parent == canvas) {
            break;
        }
    }
    update_mask_subtree(world, e, canvas_size, clip, apply_mask);
}

static void update_masks_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        update_masks(world, it->entities[i], 1);
    }
}

// A newly attached child needs its inherited clip even if its position did not change.
static void mask_parent_changed(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        if (zox_has(e, LayoutClip) || zox_has(e, Mask) ||
            zox_get_parent_by_id(world, e, zox_id(Mask))) {
            update_masks(world, e, 1);
        }
    }
}

static void mask_removed(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        update_masks(world, it->entities[i], 0);
    }
}

static void zox_observers_masks(ecs* world) {
    zox_observer(mask_layout_changed, EcsOnSet, [in] transforms2.Position2, [none] masks.Mask);
    zox_observer(mask_layout_changed, EcsOnSet, [in] layouts.LayoutSize, [none] masks.Mask);
    zox_on_add(mask_layout_changed, [none] layouts.LayoutSizeDirty, [none] masks.Mask);
    zox_on_add(mask_layout_changed, [none] masks.Mask);
    zox_observer(mask_removed, EcsOnRemove, [none] masks.Mask);
    zox_observer(mask_parent_changed, EcsOnSet, [none] Parent, [none] layouts.LayoutSize);
    zox_on_add(mask_parent_changed, [none] (ChildOf, *), [none] layouts.LayoutSize);
    zox_on_add(update_masks_event, [none] core.Dirty, [none] masks.Mask, [none] layouts.LayoutSize);
    zox_on_add(update_masks_event, [none] core.Dirty, [none] layouts.Canvas);
}
