// Most
zox_tag(Label);
zox_tag(Button);
zox_tag(Elementbar);
zox_tag(BarFront);
// Slider
zoxc_float2(SlideBounds);
zoxc(SliderLabel, const char*);
// Toggle
zox_tag(Toggle);
zox_tag(ToggleLink);
// zoxc_byte(ToggleState);
// zoxc_state(ToggleStateDirty);
// Frames
zox_tag(Frame);
zox_tag(Icon);
zox_tag(IconOverlay);
zoxc_byte(IconType);
zoxc_byte(IconIndex);
zoxc_entity(LabelPrefabLink);
zoxc_entity(FramePrefabLink);
// Scroll
zox_tag(Scrollbar);
zox_tag(ScrollbarHandle);
zoxc_entity(ScrollviewLink);
zoxc_entity(ListUILink);
zoxc(OptionLabel, const char*);

void zox_components_elements2(ecs *world) {
    zoxd_tag(Label);
    zoxd_tag(Button);
    zoxd_tag(Icon);
    zoxd_tag(Frame);
    zoxd_tag(IconOverlay);
    zoxd_tag(Elementbar);
    zoxd_tag(BarFront);
    zoxd_byte(IconType);
    zoxd_byte(IconIndex);
    zoxd_entity(LabelPrefabLink);
    zoxd_entity(FramePrefabLink);
    zoxd_float2(SlideBounds);
    zoxd(SliderLabel);
    zoxd_tag(Scrollbar);
    zoxd_tag(ScrollbarHandle);
    zoxd_entity(ScrollviewLink);
    zoxd_entity(ListUILink);
    zoxd_tag(Toggle);
    zoxd_nf_tag(ToggleLink);
    //zoxd_byte(ToggleState);
    //zoxd_state(ToggleStateDirty);
    zoxd(OptionLabel);
}
