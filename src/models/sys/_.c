#include "colors.c"
#include "fill.c"
#include "paint.c"
#include "merge.c"
#include "size.c"
#include "end.c"
#include "settings.c"
#include "renderer_lod.c"

void zox_systems_models(ecs* world) {
    zox_system(
        renderer_lod_system,
        zoxp_update,
        [in] rendering.RenderDepthDirty,
        [in] rendering.RenderDepth,
    );
    zox_system(
        model_size_node_system,
        zoxp_update,
        [out] core.Seed,
        [out] models.ModelSize,
        [none] nodes.NodeRun,
        [none] models.ModelRun,
        [none] core.Begin,
    );
    zox_system(
        fill_model_node_system,
        zoxp_update,
        [in] models.ModelSize,
        [none] nodes.NodeRun,
        [none] models.ModelRun,
        [none] core.Begin,
    );
    zox_system(
        paint_model_node_system,
        zoxp_update,
        [in] models.ModelSize,
        [none] nodes.NodeRun,
        [none] models.ModelRun,
        [none] core.Begin,
    );
    zox_system(
        model_end_node_system,
        zoxp_update,
        [none] nodes.NodeRun,
        [none] models.ModelRun,
        [none] core.Begin,
    );
    zox_system(
        colors_model_node_system,
        zoxp_update,
        [out] core.Seed,
        [none] nodes.NodeRun,
        [none] models.ModelRun,
        [none] core.Begin,
    );
    zox_system_1(
        models_settings_system,
        zoxp_spawn,
        [in] settings.LoadSettings,
    );
    zox_system(
        models_settings_dirty_system,
        zoxp_update,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
