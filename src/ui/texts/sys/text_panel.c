// this needs textData with Mesh in one entity
void ZextBackgroundSystem(iter *it) {
    zox_sys_world()
    zox_sys_begin()
    zox_sys_in(TextDirty)
    zox_sys_in(TextData)
    zox_sys_in(TextSize)
    zox_sys_in(TextPadding)
    zox_sys_in(MeshAlignment)
    zox_sys_in(CanvasLink)
    zox_sys_out(LayoutSize)
    zox_sys_out(TextureSize)
    zox_sys_out(GenerateTexture)
    zox_sys_out(MeshVertices2D)
    zox_sys_out(MeshDirty)
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TextDirty, zextDirty)
        zox_sys_i(TextData, textData)
        zox_sys_i(TextSize, textSize)
        zox_sys_i(TextPadding, textPadding)
        zox_sys_i(MeshAlignment, meshAlignment)
        zox_sys_i(CanvasLink, canvasLink)
        zox_sys_o(GenerateTexture, generateTexture)
        zox_sys_o(LayoutSize, pixelSize)
        zox_sys_o(TextureSize, textureSize)
        zox_sys_o(MeshVertices2D, meshVertices2D)
        zox_sys_o(MeshDirty, meshDirty)

        if (zextDirty->value != zox_dirty_active) {
            continue;
        }
        const int2 canvas_size = zox_get_value(canvasLink->value, LayoutSize)
        const int2 size = calculate_zext_size(textData->value, textData->length, textSize->value, textPadding->value, default_line_padding);
        const float2 canvasSizef = { (float) canvas_size.x, (float) canvas_size.y };
        const float2 size2D = (float2) { size.x / canvasSizef.y, size.y / canvasSizef.y };
        pixelSize->value = size;
        textureSize->value = size;
        set_mesh_vertices_scale2D(meshVertices2D, get_aligned_mesh2D(meshAlignment->value), 4, size2D);
        meshDirty->value = mesh_state_trigger;
        generateTexture->value = zox_generate_texture_trigger;
    }
} zoxd_system2(ZextBackgroundSystem);

void ZextParentBackgroundSystem(iter *it) {
    zox_sys_world()
    zox_sys_begin()
    zox_sys_in(TextDirty)
    zox_sys_in(TextData)
    zox_sys_in(TextSize)
    zox_sys_in(TextPadding)
    zox_sys_in(MeshAlignment)
    zox_sys_in(ParentLink)
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TextDirty, zextDirty)
        zox_sys_i(ParentLink, parentLink)
        zox_sys_i(TextData, textData)
        zox_sys_i(TextSize, textSize)
        zox_sys_i(TextPadding, textPadding)
        zox_sys_i(MeshAlignment, meshAlignment)

        if (zextDirty->value != zox_dirty_active) {
            continue;
        }
        const entity e2 = parentLink->value;
        if (!zox_has(e2, ZextLabel)) {
            continue;
        }
        const byte generate_texture = zox_get_value(e2, GenerateTexture)
        if (generate_texture) {
            return;
        }
        const int2 size = calculate_zext_size(textData->value, textData->length, textSize->value, textPadding->value, default_line_padding);

        zox_set(e2, LayoutSize, { size });
        zox_set(e2, LayoutSizeDirty, { zox_dirty_trigger });
        // on_element_pixels_resized(world, e2, size, meshAlignment->value);
    }
} zoxd_system2(ZextParentBackgroundSystem);
