#include "spawn.c"
#include "text_panel.c"
#include "text_panel_child.c"
#include "update.c"
#include "animate.c"
#include "positions.c"

void define_systems_texts(ecs *world) {
    zox_system(
        AnimateTextSystem,
        zox_pipelines_zext_textures,
        [out] AnimateZext,
        [out] texts.TextDirty,
        [out] texts.TextData
    );
    zox_system(
        TextUpdateSystem,
        EcsOnUpdate,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [none] Zext
    );
    zox_system(
        ZigelPositionSystem,
        EcsOnUpdate,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] texts.TextAlignment,
        [in] texts.TextPadding
    );
    zox_system(
        TextParentBackgroundSystem,
        zox_pipelines_zext_backgrounds,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] TextPadding,
        // [in] rendering.MeshAlignment,
        [in] hierarchys.ParentLink,
        [none] Zext
    );
    zox_system(
        TextBackgroundSystem,
        zox_pipelines_zext_backgrounds,
        [in] texts.TextDirty,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] TextPadding,
        [out] layouts2.LayoutSize,
        [out] layouts2.LayoutSizeDirty,
        [none] Zext
    );
    zox_system_1(
        ZigelSpawnSystem,
        EcsOnUpdate,
        [in] texts.TextData,
        [in] texts.TextFontSize,
        [in] TextPadding,
        [in] layouts2.Layer2D,
        [in] layouts2.CanvasPosition,
        [in] layouts2.LayoutSize,
        [in] rendering.MeshAlignment,
        [in] zigels.FontOutlineColor,
        [in] zigels.FontFillColor,
        [in] zigels.FontThickness,
        [in] zigels.FontOutlineThickness,
        [in] texts.TextResolution,
        [in] texts.TextDirty,
        [out] rendering.RenderDisabled,
        [none] Zext,
        [none] Text2D
    );
}
