zox_tag(Element);
zox_tag(Element2D);
zox_tag(ElementRaycaster);
zox_tag(ElementBillboard);
zox_tag(CanvasOverlay);
zox_tag(Window);
zox_tag(WindowRaycastTarget);
zox_tag(ElementRender);
zox_tag(MouseElement);
zox_tag(Tooltip);
zoxc_byte(ElementFontSize);
zoxc_float(ElementBar);
zoxc_float2(ElementBarSize);
zoxc_double(NavigatorTimer);
zoxc_int2(ElementMargins);
zoxc_byte(HeaderHeight);
zoxc_entity(WindowRaycasted);
zoxc_entity(WindowTarget);
zoxc_byte(WindowsLayers);
zoxc_byte(WindowsCount);
zoxc_byte(SetWindowLayer);
zoxc_byte(WindowLayer);
zoxc_byte(ElementLayer);
zoxc_state(InitializeElement);
zox_tag(ClickMakeSound);
zox_tag(Selectable);
zox_tag(Clickable);
zox_tag(Dragable);
zox_tag(IgnoreWindowLayering);
zoxc_int2(DraggingDelta);
zoxc_entity(Clicker);
zoxc_entity(ClickingEntity);
zoxc_entity(DraggerLink);
zoxc_entity(DraggedLink);
zoxc_entity(ElementLink);
zoxc_entity(UIHolderLink);
zoxc_entities(ElementLinks)
zoxc_child(ElementHolder, ElementLinks)
zoxc_fixed_string(TooltipText, 128);
// states
zoxc_byte(NavigatorState);
zoxc_byte(DraggableState);
zoxc_byte(SelectState);
zoxc_byte(ActiveState);
zoxc_byte(ClickState);
zoxc_state(ActiveStateDirty);

#include "click_event.c"
#include "tooltip_event.c"
#include "slide_event.c"
#include "toggle_event.c"

// move to hlp sub folder
entity get_linked_canvas(ecs* world, entity e) {
    return zox_valid(e) && zox_has(e, CanvasLink) ? zox_gett_value(e, CanvasLink) : 0;
}

void define_components_elements(ecs *world) {
    zoxd_tag(Element);
    zoxd_tag(Element2D);
    zoxd_tag(ElementRaycaster);
    zoxd_tag(CanvasOverlay);
    zoxd_tag(Window);
    zoxd_tag(WindowRaycastTarget);
    zoxd_tag(ElementRender);
    zoxd_tag(MouseElement);
    zoxd_tag(IgnoreWindowLayering);
    zoxd_tag(Tooltip);
    zoxd_byte(ElementFontSize);
    zoxd_float(ElementBar);
    zoxd_int2(ElementMargins);
    zoxd_float2(ElementBarSize);
    zoxd(NavigatorTimer);
    zoxd_byte(HeaderHeight);
    zoxd_entity(WindowRaycasted);
    zoxd_entity(WindowTarget);
    zoxd_byte(WindowsLayers);
    zoxd_byte(WindowsCount);
    zoxd_byte(SetWindowLayer);
    zoxd_byte(WindowLayer);
    zoxd_byte(ElementLayer);
    zoxd_state(InitializeElement);
    zoxd_tag(ClickMakeSound);
    zoxd_tag(Selectable);
    zoxd_tag(Clickable);
    zoxd_tag(Dragable);
    zoxd_entity(ClickingEntity);
    zoxd_entity(Clicker);
    zoxd_entity(ElementLink);
    zoxd_int2(DraggingDelta);
    zoxd_entity(DraggerLink);
    zoxd_entity(DraggedLink);
    zoxd_fixed_string(TooltipText);
    zoxd(UIHolderLink);
    zoxd_entities(ElementLinks);
    zox_define_component_entities_child(ElementHolder);

    zoxd(ClickEvent);
    zoxd(TooltipEvent);
    zoxd(SlideEvent);

    // States
    zoxd_byte(DraggableState);
    zoxd_byte(NavigatorState);
    zoxd_byte(SelectState);
    zoxd_byte(ActiveState);
    zoxd_byte(ClickState);
    zoxd_state(ActiveStateDirty);
}