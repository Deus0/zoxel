// this needs textData with Mesh in one entity
void ZextBackgroundSystem(iter *it) {

    zox_sys_begin();
    zox_sys_in(TextDirty);
    zox_sys_in(TextData);
    zox_sys_in(TextSize);
    zox_sys_in(TextPadding);
    // zox_sys_in(MeshAlignment);
    // zox_sys_in(CanvasLink);
    zox_sys_out(LayoutSize);
    zox_sys_out(LayoutSizeDirty);
    // zox_sys_out(TextureSize);
    // zox_sys_out(GenerateTexture);
    // zox_sys_out(MeshVertices2D);
    // zox_sys_out(MeshDirty);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(TextDirty, text_dirty);
        zox_sys_i(TextData, text);
        zox_sys_i(TextSize, size);
        zox_sys_i(TextPadding, padding);
        // zox_sys_i(MeshAlignment, alignment);
        // zox_sys_i(CanvasLink, canvas);
        zox_sys_o(LayoutSize, layout_size);
        zox_sys_o(LayoutSizeDirty, layout_size_dirty);
        // zox_sys_o(GenerateTexture, generate_texture);
        // zox_sys_o(TextureSize, texture_size);
        // zox_sys_o(MeshVertices2D, verts);
        // zox_sys_o(MeshDirty, mesh_dirty);

        if (text_dirty->value != zox_dirty_active) {
            continue;
        }

        // zox_geter_value(canvas->value, LayoutSize, int2, canvas_size);

        const int2 new_size = calculate_zext_size(
            text->value,
            text->length,
            size->value,
            padding->value,
            default_line_padding);

        layout_size->value = new_size;
        layout_size_dirty->value = zox_dirty_trigger;

        /*const float2 size2D = (float2) {
            size.x / (float) canvas_size.y,
            size.y / (float) canvas_size.y
        };*/
        // texture_size->value = new_size;
        // set_mesh_vertices_scale2D(verts, get_aligned_mesh2D(alignment->value), 4, size2D);
        // mesh_dirty->value = mesh_state_trigger;
        // generate_texture->value = zox_generate_texture_trigger;
    }
} zoxd_system2(ZextBackgroundSystem);
