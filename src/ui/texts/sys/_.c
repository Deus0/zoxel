#include "state.c"
#include "spawn.c"
#include "text_panel.c"
#include "text_panel_child.c"
#include "positions.c"
#include "update.c"
#include "animate.c"

void zox_systems_texts(ecs *world) {
    // types TargetText out a character at a time (i.e. dialogue)
    zox_system(
        animate_text_system,
        zoxp_update,
        [in] texts.TargetText,
        [in] texts.AnimateTextTimeLimits,
        [out] texts.AnimateTextBegin,
        [out] texts.AnimateTextTime,
        [out] texts.TextData,
        [out] texts.GlyphSpawnedDirty,
        [out] texts.AnimateTextEnded,
        [none] texts.Text,
    );
    zox_system(
        text_parent_background_system,
        zoxp_update,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] TextPadding,
        [none] texts.Text,
        [none] core.Update,
    );
    zox_system(
        text_background_system,
        zoxp_update,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] TextPadding,
        [out] layouts.LayoutSize,
        [none] texts.Text,
        [none] !texts.TextWrap,
        [none] core.Update,
    );
    zox_system_1(
        glyph_spawn_system,
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
        text_update_system,
        zoxp_update,
        [in] texts.TextData,
        [none] texts.Text,
        [none] core.Update,
    );
    zox_system(
        texts_position_system,
        zoxp_update,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] texts.TextAlignment,
        [in] texts.TextPadding,
        [none] texts.Text,
        [none] core.Update,
    );
}
