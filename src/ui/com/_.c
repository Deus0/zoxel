// Core Elements
zox_tag(Element);
zox_tag(Element2D);
// zox_tag(ElementRender);
zoxc_byte(ElementLayer);
zoxc_entity(ElementLink);
zoxc_entities(ElementLinks)
zoxc_child(ElementHolder, ElementLinks)
zoxc_entity(UIHolderLink);
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
zoxc_float2(ElementBarSize);
zoxc_byte(FontSize);
zoxc_byte(ElementFontSize);
// Misc
zox_tag(MouseElement);

void zox_define_components_elements(ecs *world) {
    zoxd_tag(Element);
    zoxd_tag(Element2D);
    zoxd_byte(ElementLayer);
    zoxd_entity(ElementLink);
    zoxd_entities(ElementLinks);
    zoxd(UIHolderLink);
    zoxd_child(ElementHolder);
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
    zoxd_float2(ElementBarSize);
    // Canvas
    zoxd_tag(CanvasOverlay);
}
