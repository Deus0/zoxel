#include "plot_lines.c"
#include "plot_labels.c"
#include "plot_limits.c"

void define_systems_plots(ecs* world) {
    zox_system(
        plot_line_system,
        zoxp_update,
        [in] plots.PlotLineIndex,
        [out] layouts.LayoutLinePoints,
        [none] plots.PlotLine,
    );
    zox_system(
        PlotLabelSystem,
        zoxp_update,
        [out] texts.TextData,
        [none] texts.Text,
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
