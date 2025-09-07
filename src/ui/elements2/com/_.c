zox_tag(Label);
zox_tag(Button);
zox_tag(CloseButton);
zox_tag(Icon);
zox_tag(Frame);
zox_tag(IconOverlay);
zox_tag(Header);
zox_tag(Body);
zox_tag(Scrollbar);
zox_tag(ScrollbarButton);
zox_tag(Elementbar2);
zoxc_byte(IconType);
zoxc_byte(IconIndex);
zoxc_entity(LabelPrefabLink);
zoxc_entity(FramePrefabLink);
zoxc_float2(SlideBounds);
zoxc(SliderLabel, const char*);

void define_components_elements2(ecs *world) {
    zoxd_tag(Label);
    zoxd_tag(Button);
    zoxd_tag(Icon);
    zoxd_tag(Frame);
    zoxd_tag(IconOverlay);
    zoxd_tag(Header);
    zoxd_tag(Body);
    zoxd_tag(CloseButton);
    zoxd_tag(Scrollbar);
    zoxd_tag(ScrollbarButton);
    zoxd_tag(Elementbar2);
    zoxd_byte(IconType);
    zoxd_byte(IconIndex);
    zoxd_entity(LabelPrefabLink);
    zoxd_entity(FramePrefabLink);
    zoxd_float2(SlideBounds);
    zoxd(SliderLabel);
}