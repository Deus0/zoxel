#include "state.c"
#include "spawn.c"
#include "text_panel.c"
#include "text_panel_child.c"
#include "positions.c"
#include "update.c"
// #include "animate.c"

void define_systems_texts(ecs *world) {
    /*zox_system(
        text_dirty_system,
        zoxp_remove,
        [none] texts.Text,
        [none] texts.TextDirty,
    );*/
    // TODO: Move Dialogue one here
    zox_system(
        TextParentBackgroundSystem,
        zoxp_update,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] TextPadding,
        [none] texts.Text,
        [none] core.Update,
    );
    zox_system(
        TextBackgroundSystem,
        zoxp_update,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] TextPadding,
        [out] layouts.LayoutSize,
        [out] layouts.LayoutSizeDirty,
        [none] texts.Text,
        [none] core.Update,
    );
    zox_system_1(
        GlyphSpawnSystem,
        zoxp_spawn,
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
        [none] core.Update,
    );
    zox_system(
        TextUpdateSystem,
        zoxp_update,
        [in] texts.TextData,
        [none] texts.Text,
        [none] core.Update,
    );
    zox_system(
        TextsPositionSystem,
        zoxp_update,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] texts.TextAlignment,
        [in] texts.TextPadding,
        [none] texts.Text,
        [none] core.Update,
    );
}