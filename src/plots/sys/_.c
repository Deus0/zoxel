#include "plot_lines.c"
#include "plot_labels.c"
#include "plot_limits.c"

void define_systems_plots(ecs* world) {
    zox_system(
        PlotLineSystem,
        zoxp_update,
        [in] plots.PlotLineIndex,
        [out] lines2.LineLocalPosition2,
        [out] lines2.LineLocalPositionDirty,
        [none] plots.PlotLine
    );
    zox_system(
        PlotLabelSystem,
        zoxp_update,
        [out] texts.TextDirty,
        [out] texts.TextData,
        [none] plots.PlotLabel
    );
    zox_system(
        PlotLimitSystem,
        zoxp_update,
        [in] plots.PlotDataLink,
        [out] plots.PlotMin,
        [out] plots.PlotMax,
        [none] plots.Plot
    );
}
