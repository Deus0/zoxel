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
        unfold_toggle(world, owner, zox_getv(arrow, Target));
    }
}

entity spawn_fold_arrow(ecs* world, entity canvas, entity row, entity owner, entity target, byte unfolded, byte size, byte layer) {
    const entity arrow = spawn_button_icon(world, prefab_clickable, fold_arrow_textures[!!unfolded], canvas, row,
        (int2) { size / 2 + 2, 0 }, int2_single(size), (float2) { 0, 0.5f }, layer,
        empty_color, empty_color, 0, (ClickEvent) { click_fold_arrow }, 4);
    zox_setv(arrow, Target, target);
    zox_set_link(world, arrow, FoldOwnerLink, owner);
    return arrow;
}
