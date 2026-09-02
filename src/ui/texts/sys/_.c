#include "spawn.c"
#include "text_panel.c"
#include "text_panel_child.c"
// #include "animate.c"
#include "positions.c"
#include "update.c"

void define_systems_texts(ecs *world) {
    // TODO: Move Dialogue one here
    zox_system(
        TextParentBackgroundSystem,
        zoxp_update,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] TextPadding,
        [none] texts.Text
    );
    zox_system(
        TextBackgroundSystem,
        zoxp_update,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] TextPadding,
        [out] layouts.LayoutSize,
        [out] layouts.LayoutSizeDirty,
        [none] texts.Text
    );
    zox_system_1(
        GlyphSpawnSystem,
        zoxp_spawn,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [in] texts.GlyphPrefab,
        [in] texts.TextFontSize,
        [in] glyphs.FontOutlineColor,
        [in] glyphs.FontFillColor,
        [in] glyphs.FontThickness,
        [in] textures.OutlineThickness,
        [in] texts.TextResolution,
        [in] layouts.Layer,
        [out] rendering.RenderDisabled,
        [none] texts.Text,
    );
    zox_system(
        TextUpdateSystem,
        zoxp_update,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [none] texts.Text,
    );
    zox_system(
        TextsPositionSystem,
        zoxp_update,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] texts.TextAlignment,
        [in] texts.TextPadding,
        [none] texts.Text,
    );
}
    /*
    zox_system(
        GlyphPositionSystem,
        zoxp_update,
        [in] hierarchys.ChildIndex,
        [out] glyphs.GlyphDirty,
        [out] layouts.LayoutPosition,
        [out] layouts.LayoutPositionDirty,
        [none] glyphs.Glyph
    );
    zox_system(
        GlyphUpdateSystem,
        zoxp_update,
        [in] hierarchys.ChildIndex,
        [out] glyphs.GlyphDirty,
        [out] glyphs.GlyphIndex,
        [out] textures.GenerateTexture,
        [none] glyphs.Glyph
    );*/

