zox_tag(Plot);
zox_tag(PlotLine);
zox_tag(PlotLabel);
zoxc_byte(PlotPaused);
zoxc_double(PlotMin);
zoxc_double(PlotMax);
zoxc_entity(PlotDataLink);
zoxc_int(PlotLineIndex);

void zox_components_plots(ecs* world) {
    zoxd_tag(Plot);
    zoxd_tag(PlotLine);
    zoxd_tag(PlotLabel);
    zoxd_byte(PlotPaused);
    zoxd_double(PlotMin);
    zoxd_double(PlotMax);
    zoxd_entity(PlotDataLink);
    zoxd_int(PlotLineIndex);
}
