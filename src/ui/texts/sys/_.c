#include "spawn.c"
#include "text_panel.c"
#include "text_panel_child.c"
#include "update.c"
#include "animate.c"
#include "positions.c"

void define_systems_texts(ecs *world) {
    // TODO: Move Dialogue one here
    zox_system(
        TextParentBackgroundSystem,
        zoxp_update,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] TextPadding,
        [none] texts.Zext
    );
    zox_system(
        TextBackgroundSystem,
        zoxp_update,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] TextPadding,
        [out] layouts2.LayoutSize,
        [out] layouts2.LayoutSizeDirty,
        [none] texts.Zext
    );
    zox_system(
        TextUpdateSystem,
        zoxp_update,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [none] texts.Zext
    );
    /*zox_system(
        TextsPositionSystem,
        zoxp_update,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] texts.TextAlignment,
        [in] texts.TextPadding,
        [none] texts.Zext
    );*/
    zox_system(
        ZigelPositionSystem,
        zoxp_update,
        [in] zigels.ZigelIndex,
        [in] zigels.DataIndex,
        [in] layouts2.LayoutPosition,
        [in] layouts2.LayoutPositionDirty,
        [none] zigels.Zigel
    );
    zox_system_1(
        ZigelSpawnSystem,
        zoxp_mainthread,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] zigels.FontOutlineColor,
        [in] zigels.FontFillColor,
        [in] zigels.FontThickness,
        [in] zigels.FontOutlineThickness,
        [in] texts.TextResolution,
        [in] layouts2.Layer2D,
        [out] rendering.RenderDisabled,
        [none] texts.Zext,
        [none] texts.Text2D
    );
}
