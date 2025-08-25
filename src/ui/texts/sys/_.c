#include "resize.c"
#include "text_panel.c"
#include "update.c"
#include "animate.c"
zox_increment_system_with_reset(TextDirty, zext_update_end);

void define_systems_texts(ecs *world) {
    zoxd_system_increment(TextDirty, [none] Zext);
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
        [in] hierarchys.Children,
        [none] texts.Zext
    );
    if (!headless) {
        zox_system(
            ZextParentBackgroundSystem,
            zox_pipelines_zext_backgrounds,
            [in] texts.TextDirty,
            [in] texts.TextData,
            [in] texts.TextSize,
            [in] TextPadding,
            [in] rendering.MeshAlignment,
            [in] hierarchys.ParentLink,
            [none] Zext
        );
        zox_system(
            ZextBackgroundSystem,
            zox_pipelines_zext_backgrounds,
            [in] texts.TextDirty,
            [in] texts.TextData,
            [in] texts.TextSize,
            [in] TextPadding,
            [in] rendering.MeshAlignment,
            [in] layouts2.CanvasLink,
            [out] layouts2.LayoutSize,
            [out] rendering.TextureSize,
            [out] textures.GenerateTexture,
            [out] rendering.MeshVertices2D,
            [out] rendering.MeshDirty,
            [none] Zext
        );
    }
    zox_system_1(
        TextResizeSystem,
        EcsPreStore,
        [in] texts.TextData,
        [in] texts.TextSize,
        [in] TextPadding,
        [in] layouts2.Layer2D,
        [in] layouts2.CanvasPosition,
        [in] layouts2.LayoutSize,
        [in] rendering.MeshAlignment,
        [in] zigels.FontOutlineColor,
        [in] zigels.FontFillColor,
        [in] zigels.FontThickness,
        [in] zigels.FontOutlineThickness,
        [in] TextResolution,
        [in] texts.TextDirty,
        [out] rendering.RenderDisabled,
        [out] hierarchys.Children,
        [none] Zext,
        [none] Text2D
    );
}