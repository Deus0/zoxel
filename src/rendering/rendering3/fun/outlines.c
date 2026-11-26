// outlines (doc/plans/outlines.md): the vox draws between these write the outline mask, their outline id (1, the
//  model's own dark shade), only while the camera drawing has outlines (render_outlines)
static inline void outline_mask_begin(gint outline) {
    if (!render_outlines) {
        return;
    }
    zox_gpu_outline_mask(1);
    zox_gpu_float(outline, 1 / 255.0f);
}

static inline void outline_mask_end() {
    if (render_outlines) {
        zox_gpu_outline_mask(0);
    }
}
