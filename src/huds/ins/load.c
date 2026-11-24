// the "Loading" ui: "Loading" and under it the near chunks loaded (Chunks [loaded/near], load_ui_chunks_system), the
//  same texts in 2D and on 3D huds (XR), their parent's the canvas's child either way (huds/eve/load.c finds it, LoadUI,
//  deleting it deletes both)
//  - 2D: a layout on the canvas, in its middle; a layout, as a text's children are its glyphs
//  - 3D: a canvas3 ahead of the head (huds3_follow_system), its texts the 3D twins (spawn_text, ui_prefab_for)

// the texts in it, the same in 2D and 3D: size is the parent's, to fit them
static inline void spawn_load_ui_texts(ecs* world, entity parent, byte font_size, byte chunks_font_size, byte is3D) {
    const color fill = (color) { 44, 222, 222, 222 };
    const color outline = (color) { 0, 255, 255, 244 };
    const byte2 padding = byte2_single(1 * ui_scale);
    const entity label = spawn_text(world, prefab_text, parent, int2_zero, float2_half, font_size,
        zox_alignment_centre, padding, "Loading", fill, outline);
    const entity chunks = spawn_text(world, prefab_text, parent, (int2) { 0, -(font_size + chunks_font_size) },
        float2_half, chunks_font_size, zox_alignment_centre, padding, "Chunks [0/0]", fill, outline);
    zox_add(chunks, LoadUIChunks);
    if (!is3D) {
        zox_setv(label, Layer, get_game_overlay_layer());
        zox_setv(chunks, Layer, get_game_overlay_layer());
    }
}

entity spawn_load_ui(
    ecs* world,
    entity player,
    entity canvas)
{
    if (!zox_valid(canvas)) {
        return 0;
    }
    const byte font_size = ui_scale * 24;
    const byte chunks_font_size = ui_scale * 12;
    const int2 size = { font_size * 8, font_size * 2 + chunks_font_size * 2 };
    entity e;
    if (zox_huds3D) {
        // "Loading" about 7cm tall at huds3_distance, alone on black, read at a glance
        e = huds3_spawn_root(world, player, canvas, size, huds3_label_height * 1.2f * size.y / font_size);
    } else {
        e = spawn_layout2(world, prefab_layout2, canvas, int2_zero, size, float2_half, get_game_overlay_layer());
    }
    spawn_load_ui_texts(world, e, font_size, chunks_font_size, zox_huds3D);
    zox_stream_end_loaded = 0;  // not the last load's, till stream_end_system counts this one
    zox_stream_end_near = 0;
    zox_set_unique_name(e, "load_ui");
    zox_add(e, LoadUI);
    return e;
}
