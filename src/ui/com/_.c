// Core Elements
zox_tag(Element);
zox_tag(Element2D);
zox_tag(ElementRender);
zoxc_byte(ElementLayer);
zoxc_state(InitializeElement);
zoxc_entity(ElementLink);
zoxc_entities(ElementLinks)
zoxc_child(ElementHolder, ElementLinks)
zoxc_entity(UIHolderLink);
zoxc_int2(ElementMargins);
zox_tag(ElementBillboard);

// Selecting
zox_tag(Selectable);
zoxc_byte(SelectState);
zox_tag(ElementRaycaster);

// Clicking
zox_tag(Clickable);
zox_tag(ClickMakeSound);
zoxc_byte(ClickState);
zoxc_entity(Clicker);
zoxc_entity(ClickingEntity);

// Dragging
zox_tag(Dragable);
zoxc_int2(DraggingDelta);
zoxc_entity(DraggerLink);
zoxc_entity(DraggedLink);
zoxc_byte(DraggableState);

// Active
zoxc_byte(ActiveState);
zoxc_state(ActiveStateDirty);

// Navigating
zoxc_byte(NavigatorState);
zoxc_double(NavigatorTimer);

// TODO: Move to Sub Modules

// Bars
zoxc_float(ElementBar);
zoxc_float2(ElementBarSize);
zoxc_byte(ElementFontSize);

// Windows
zox_tag(Window);
zox_tag(WindowRaycastTarget);
zox_tag(IgnoreWindowLayering);
zoxc_byte(WindowLayer);
zoxc_byte(SetWindowLayer);
zoxc_entity(WindowRaycasted);
zoxc_entity(WindowTarget);
zoxc_byte(WindowsLayers);
zoxc_byte(WindowsCount);
zoxc_byte(HeaderHeight);

// Tooltips
zox_tag(Tooltip);
zoxc_fixed_string(TooltipText, 128);

// Misc
zox_tag(MouseElement);
zox_tag(CanvasOverlay);

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
    zoxd_entity(ClickingEntity);
    zoxd_entity(Clicker);
    zoxd_entity(ElementLink);
    zoxd_fixed_string(TooltipText);
    zoxd(UIHolderLink);
    zoxd_entities(ElementLinks);
    zox_define_component_entities_child(ElementHolder);

    zoxd(ClickEvent);
    zoxd(TooltipEvent);
    zoxd(SlideEvent);

    // States
    zoxd_byte(NavigatorState);
    zoxd_byte(SelectState);
    zoxd_byte(ActiveState);
    zoxd_byte(ClickState);
    zoxd_state(ActiveStateDirty);

    // Dragging
    zoxd_tag(Dragable);
    zoxd_int2(DraggingDelta);
    zoxd_entity(DraggerLink);
    zoxd_entity(DraggedLink);
    zoxd_byte(DraggableState);
}