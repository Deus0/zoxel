typedef struct  {
    byte event;
    entity ui;
    entity tooltip;
    entity data;
} TooltipEventData;

zox_tag(Tooltip);
zox_tag(Tooltipper);
zox_tag(TooltipLink);
zoxc_entity(TooltipperLink);
zoxc_fixed_string(TooltipText, 512);
zoxc_function(TooltipEvent, byte, ecs*, const TooltipEventData*);

void zox_define_components_tooltips(ecs* world) {
    zoxd_tag(Tooltip);
    zoxd_tag(Tooltipper);
    zoxd_tag(TooltipLink);
    zoxd_entity(TooltipperLink);
    zoxd_fixed_string(TooltipText);
    zoxd(TooltipEvent);
}
