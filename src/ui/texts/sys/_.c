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
        [none] texts.Zext
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
        [none] texts.Zext
    );
    zox_system_1(
        ZigelSpawnSystem,
        zoxp_mainthread,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [in] texts.ZigelPrefab,
        [in] texts.TextFontSize,
        [in] zigels.FontOutlineColor,
        [in] zigels.FontFillColor,
        [in] zigels.FontThickness,
        [in] zigels.FontOutlineThickness,
        [in] texts.TextResolution,
        [in] layouts.Layer2D,
        [out] rendering.RenderDisabled,
        [none] texts.Zext,
        [none] texts.Text2D
    );
    /*
    zox_system(
        ZigelPositionSystem,
        zoxp_update,
        [in] hierarchys.ChildIndex,
        [out] zigels.ZigelDirty,
        [out] layouts.LayoutPosition,
        [out] layouts.LayoutPositionDirty,
        [none] zigels.Zigel
    );
    zox_system(
        ZigelUpdateSystem,
        zoxp_update,
        [in] hierarchys.ChildIndex,
        [out] zigels.ZigelDirty,
        [out] zigels.ZigelIndex,
        [out] textures.GenerateTexture,
        [none] zigels.Zigel
    );*/
    zox_system(
        TextUpdateSystem,
        zoxp_update,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [none] texts.Zext,
    );
    zox_system(
        TextsPositionSystem,
        zoxp_update,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] texts.TextAlignment,
        [in] texts.TextPadding,
        [none] texts.Zext,
    );
}

