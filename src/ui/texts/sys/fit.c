// fits a TextFit text in its TextFitWidth: the font is TextFitFontSize, smaller when the text would be wider, and its
//  parent (an input field's button) is sized to it, as typing changes it
//  - a new font size respawns its glyphs (glyph_spawn_system), they're sized as they spawn
void text_fit_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextData);
    zox_sys_in(TextPadding);
    zox_sys_in(TextFitFontSize);
    zox_sys_in(TextFitWidth);
    zox_sys_out(TextFontSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextData, text);
        zox_sys_i(TextPadding, padding);
        zox_sys_i(TextFitFontSize, full);
        zox_sys_i(TextFitWidth, width);
        zox_sys_o(TextFontSize, font_size);
        int fits = full->value;
        const int2 full_size = calculate_zext_size(text->value, text->length, full->value, padding->value, default_line_padding);
        if (width->value > 0 && full_size.x > width->value) {
            // its glyphs are font size wide, the padding stays
            const int glyphs_width = full_size.x - padding->value.x * 2;
            const int room = width->value - padding->value.x * 2;
            fits = glyphs_width > 0 ? full->value * room / glyphs_width : full->value;
            if (fits < 4) {
                fits = 4;
            }
        }
        if (fits != font_size->value) {
            font_size->value = fits;
            // its glyphs again, at the new size
            iter it2 = zox_children(world, e);
            while (zox_children_next(it2)) {
                for (int j = 0; j < it2.count; j++) {
                    if (zox_has(it2.entities[j], Glyph)) {
                        zox_delete(it2.entities[j]);
                    }
                }
            }
        }
        // its parent fits it
        const entity parent = zox_get_parent(world, e);
        if (zox_valid(parent) && zox_has(parent, LayoutSize)) {
            const int2 size = calculate_zext_size(text->value, text->length, fits, padding->value, default_line_padding);
            if (!int2_equals(size, zox_getv(parent, LayoutSize))) {
                zox_setv(parent, LayoutSize, size);
                zox_add(parent, LayoutSizeDirty);
            }
            if (zox_has(e, LayoutSize) && !int2_equals(size, zox_getv(e, LayoutSize))) {
                zox_setv(e, LayoutSize, size);
                zox_add(e, LayoutSizeDirty);
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(text_fit_system);
