typedef struct  {
    byte event;
    entity tooltip;
    entity data;
    entity triggered;
} TooltipEventData;
zox_tag(Tooltip);
zox_tag(Tooltipper);
zoxc_fixed_string(TooltipText, 512);
zoxc_function(TooltipEvent, byte, ecs*, const TooltipEventData*);

void zox_define_components_tooltips(ecs* world) {
    zoxd_tag(Tooltip);
    zoxd_tag(Tooltipper);
    zoxd_fixed_string(TooltipText);
    zoxd(TooltipEvent);
}
