#include "plot_lines.c"
#include "plot_labels.c"
#include "plot_limits.c"

void zox_systems_plots(ecs* world) {
    zox_system(
        plot_line_system,
        zoxp_update,
        [in] plots.PlotLineIndex,
        [out] layouts.LayoutLinePoints,
        [none] plots.PlotLine,
    );
    zox_system(
        plot_label_system,
        zoxp_update,
        [out] texts.TextData,
        [none] texts.Text,
        [none] plots.PlotLabel
    );
    zox_system(
        plot_limit_system,
        zoxp_update,
        [out] plots.PlotMin,
        [out] plots.PlotMax,
        [none] plots.Plot
    );
}
