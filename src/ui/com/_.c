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
zoxc_color(ElementColor);

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
zoxc_byte(ClickDisabled);

// Dragging
zox_tag(Dragable);
zoxc_int2(DraggingDelta);
zoxc_entity(DraggerLink);
zoxc_entity(DraggedLink);
zoxc_byte(DraggableState);

// Active
zoxc_byte(ActiveState);
zoxc_state(ActiveStateDirty);
zoxc_color(ActiveColor);
zox_tag(ActiveSingle);
zoxc_entity(ActiveLink);    // parents active link

// Navigating
zox_tag(NavigationWindow);
zox_tag(NavigationElement);
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
zoxc_fixed_string(TooltipText, 512);
#include "tooltip_event.c"

// Misc
zox_tag(MouseElement);
zox_tag(CanvasOverlay);

#include "click_event.c"
#include "slide_event.c"
#include "toggle_event.c"

void define_components_elements(ecs *world) {
    // Core Elements
    zoxd_tag(Element);
    zoxd_tag(Element2D);
    zoxd_tag(ElementRender);
    // zoxd_tag(ElementBillboard);
    zoxd_byte(ElementLayer);
    zoxd_state(InitializeElement);
    zoxd_entity(ElementLink);
    zoxd_entities(ElementLinks);
    zoxd(UIHolderLink);
    zoxd_child(ElementHolder);
    zoxd_int2(ElementMargins);
    zoxd_color(ElementColor);

    // Click
    zoxd_tag(Clickable);
    zoxd_tag(ClickMakeSound);
    zoxd_byte(ClickState);
    zoxd_entity(ClickingEntity);
    zoxd_entity(Clicker);
    zoxd(ClickEvent);
    zoxd_byte(ClickDisabled);

    // Selecting
    zoxd_tag(Selectable);
    zoxd_byte(SelectState);
    zoxd_tag(ElementRaycaster);

    // Active
    zoxd_byte(ActiveState);
    zoxd_state(ActiveStateDirty);
    zoxd_color(ActiveColor);
    zoxd_entity(ActiveLink);
    zoxd_tag(ActiveSingle);

    // Windows
    zoxd_tag(CanvasOverlay);
    zoxd_tag(Window);
    zoxd_tag(WindowRaycastTarget);
    zoxd_tag(IgnoreWindowLayering);

    // Tooltips
    zoxd_tag(Tooltip);
    zoxd_fixed_string(TooltipText);
    zoxd(TooltipEvent);

    // Misc
    zoxd_tag(MouseElement);
    zoxd_byte(ElementFontSize);
    zoxd_float(ElementBar);
    zoxd_float2(ElementBarSize);
    zoxd_byte(HeaderHeight);
    zoxd_entity(WindowRaycasted);
    zoxd_entity(WindowTarget);
    zoxd_byte(WindowsLayers);
    zoxd_byte(WindowsCount);
    zoxd_byte(SetWindowLayer);
    zoxd_byte(WindowLayer);

    // Slides
    zoxd(SlideEvent);

    // Navigation
    zoxd_tag(NavigationWindow);
    zoxd_tag(NavigationElement);
    zoxd_double(NavigatorTimer);
    zoxd_byte(NavigatorState);

    // Dragging
    zoxd_tag(Dragable);
    zoxd_int2(DraggingDelta);
    zoxd_entity(DraggerLink);
    zoxd_entity(DraggedLink);
    zoxd_byte(DraggableState);
}
