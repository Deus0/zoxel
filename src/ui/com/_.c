// Core Elements
zox_tag(Element);
zox_tag(Element2D);
zox_tag(ElementLink);      // links to a UI element, i.e. a zevices virtual joystick
zox_tag(ElementLinks);     // links to many UI elements, i.e. a characters 3D UIs
zox_tag(UIHolderLink);     // a 3D UI links to its holder, it is deleted with it
zoxc_int2(ElementMargins);
zoxc_color(ElementFillColor);
zoxc_color(ElementOutlineColor);
// Canvas
zox_tag(CanvasOverlay);
// Navigating
zox_tag(NavigationWindow);
zox_tag(NavigationElement);
zoxc_byte(NavigatorState);
zoxc_double(NavigatorTimer);
// Bars
zoxc_float(BarLevel);
zoxc_byte(FontSize);
zoxc_byte(ElementFontSize);
// Misc
zox_tag(MouseElement);
// Hiding: an element shrinks away after this many seconds idle, pops back in when poked (ui_poke, ui/sys/auto_hide.c)
zoxc_float(AutoHide);
zoxc_double(AutoHideTime);     // when it was last poked
zox_tag(AutoHidden);

void zox_components_elements(ecs *world) {
    zoxd_float(AutoHide);
    zoxd_double(AutoHideTime);
    zoxd_tag(AutoHidden);
    zoxd_tag(Element);
    zoxd_tag(Element2D);
    zoxd_nf_link(ElementLink);
    zoxd_nf_links(ElementLinks);
    zoxd_nf_link_owned(UIHolderLink);
    zoxd_int2(ElementMargins);
    zoxd_color(ElementFillColor);
    zoxd_color(ElementOutlineColor);
    // Navigation
    zoxd_tag(NavigationWindow);
    zoxd_tag(NavigationElement);
    zoxd_double(NavigatorTimer);
    zoxd_byte(NavigatorState);
    // Misc
    zoxd_byte(FontSize);
    zoxd_tag(MouseElement);
    zoxd_byte(ElementFontSize);
    zoxd_float(BarLevel);
    // Canvas
    zoxd_tag(CanvasOverlay);
}
