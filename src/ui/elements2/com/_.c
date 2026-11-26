#include "virtual_list.c"
// Most
zox_tag(Label);
zox_tag(Button);
zox_tag(Elementbar);
zox_tag(BarFront);
zox_tag(DragResizes);
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
zox_tag(LabelPrefabLink);
zox_tag(FramePrefabLink);
// Scroll
zox_tag(Scrollview);
zox_tag(HorizontalScrollbar);
zox_tag(Scrollbar);
zox_tag(ScrollbarHandle);
zox_tag(ScrollviewLink);
zox_tag(ListUILink);
zox_tag(FoldOwnerLink);
zox_tag(FoldArrow);
zox_tag(Unfolded);
zoxc(OptionLabel, const char*);

zox_tag(Dropdown);
zox_tag(DropdownSelectedLink);
zox_tag(ItemListCanvas);
zox_tag(OpenItemList);
zox_tag(ItemListPopup);
zox_tag(ItemListItem);
zox_tag(ItemListDisabled);
zoxc_byte(ItemListChecked);
zox_tag(ItemListTick);
zox_tag(Menu);
zox_tag(Menubar);

void zox_components_elements2(ecs *world) {
    zox_components_virtual_list(world);
    zoxd_tag(Dropdown);
    zoxd_nf_link(DropdownSelectedLink);
    zoxd_nf_link(ItemListCanvas);
    zoxd_nf_link(OpenItemList);
    zoxd_tag(ItemListPopup);
    zoxd_tag(ItemListItem);
    zoxd_tag(ItemListDisabled);
    zoxd_byte(ItemListChecked);
    zoxd_tag(ItemListTick);
    zoxd_tag(Menu);
    zoxd_tag(Menubar);
    zoxd_tag(Label);
    zoxd_tag(Button);
    zoxd_tag(Icon);
    zoxd_tag(Frame);
    zoxd_tag(IconOverlay);
    zoxd_tag(Elementbar);
    zoxd_tag(BarFront);
    zoxd_tag(DragResizes);
    zoxd_byte(IconType);
    zoxd_byte(IconIndex);
    zoxd_nf_link(LabelPrefabLink);
    zoxd_nf_link(FramePrefabLink);
    zoxd_float2(SlideBounds);
    zoxd(SliderLabel);
    zoxd_tag(Scrollview);
    zoxd_tag(HorizontalScrollbar);
    zoxd_tag(Scrollbar);
    zoxd_tag(ScrollbarHandle);
    zoxd_nf_link(ScrollviewLink);
    zoxd_nf_link(ListUILink);
    zoxd_tag(Toggle);
    zoxd_nf_link(ToggleLink);
    //zoxd_byte(ToggleState);
    //zoxd_state(ToggleStateDirty);
    zoxd_nf_link(FoldOwnerLink);
    zoxd_tag(FoldArrow);
    zoxd_nf_links(Unfolded);
    zoxd(OptionLabel);
}
