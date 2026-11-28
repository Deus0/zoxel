static entity fold_arrow_textures[2];

static void spawn_fold_arrow_textures(ecs* world) {
    // Image UVs start at the bottom left.
    const int side = 15;
    const int middle = side / 2;
    for (int down = 0; down < 2; down++) {
        const entity texture = spawn_texture(world, prefab_texture, int2_single(side));
        TextureData pixels = { 0 };
        initialize_TextureData(&pixels, side * side);
        for (int y = 0; y < side; y++) {
            for (int x = 0; x < side; x++) {
                const int width = down ? y : side - 1 - x;
                const int offset = down ? x - middle : y - middle;
                pixels.value[y * side + x] = 2 * abs(offset) <= width ? color_white : empty_color;
            }
        }
        zox_set_ptr(texture, TextureData, pixels);
        zox_setv(texture, TextureGPULink, 0);
        zox_add(texture, Initialize);
        zox_add(texture, TextureDirty);
        fold_arrow_textures[down] = texture;
    }
}

static void click_fold_arrow(ecs* world, ClickEventData event) {
    const entity arrow = event.clicked;
    const entity owner = zox_get_link(world, arrow, FoldOwnerLink);
    if (zox_valid(owner)) {
        unfold_toggle(world, owner, zox_get_link(world, arrow, RowTargetLink));
    }
}

entity spawn_fold_arrow(ecs* world, entity canvas, entity row, entity owner, entity target, byte unfolded, byte size, byte layer) {
    const entity arrow = spawn_button_icon(world, prefab_clickable, fold_arrow_textures[!!unfolded], canvas, row,
        (int2) { size / 2 + 2, 0 }, int2_single(size), (float2) { 0, 0.5f }, layer,
        empty_color, empty_color, !target, (ClickEvent) { click_fold_arrow }, 4);  // nothing to fold: hidden
    zox_add(arrow, FoldArrow);
    zox_set_link(world, arrow, RowTargetLink, target);
    zox_set_link(world, arrow, FoldOwnerLink, owner);
    return arrow;
}


static void set_fold_arrow(ecs* world, entity arrow, entity target, byte open, byte hidden) {
    zox_set_link(world, arrow, RowTargetLink, target);
    set_children_render_disabled(world, arrow, hidden);
    if (hidden) {
        return;
    }
    const entity image = zox_get_child_by_id(world, arrow, zox_id(TextureData));
    if (!image) {
        return;     // still spawning, with the texture it was spawned with
    }
    const TextureData* source = zox_get(fold_arrow_textures[!!open], TextureData);
    zox_muter(image, TextureData, texture);
    const size_t bytes = source->length * sizeof(color);
    if (memcmp(texture->value, source->value, bytes)) {
        memcpy(texture->value, source->value, bytes);
        zox_add(image, TextureDirty);
    }
}

// an arrow spawned after its row was filled (a respawned slot's, deferred) has its row fill it again
static void fold_arrow_added_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity row = zox_get_parent(world, it->entities[i]);
        if (zox_valid(row) && zox_has(row, ListSlot)) {
            zox_add(row, Dirty);
        }
    }
}
