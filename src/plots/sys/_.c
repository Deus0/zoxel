#include "plot_lines.c"
#include "plot_labels.c"
#include "plot_limits.c"

void define_systems_plots(ecs* world) {
    zox_system(
        PlotLineSystem,
        EcsOnUpdate,
        // [in] hierarchys.ParentLink,
        [in] hierarchys.ChildIndex,
        [out] lines2.d.LineLocalPosition2,
        [none] plots.PlotLine
    );
    zox_system(
        PlotLabelSystem,
        EcsOnUpdate,
        // [in] hierarchys.ParentLink,
        [out] texts.TextDirty,
        [out] texts.TextData,
        [none] plots.PlotLabel
    );
    zox_system(
        PlotLimitSystem,
        EcsOnUpdate,
        [in] core.DataDouble,
        [out] plots.PlotMin,
        [out] plots.PlotMax,
        [none] plots.Plot
    );
}
