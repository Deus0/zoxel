// Selecting
zox_tag(Selectable);
zox_tag(SelectedBrighter);
zoxc_byte(SelectState);
zoxc_color(SelectedFillColor);
zoxc_color(SelectedOutlineColor);
zox_tag(ElementRaycaster);
// Clicking
zox_tag(Clickable);
zox_tag(ClickMakeSound);
zoxc_byte(ClickState);
zoxc_byte(ClickType);
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
// Events
zox_tag(TriggerSelect);
zox_tag(Select);

typedef struct {
    entity clicker;
    entity clicked;
} ClickEventData;

typedef struct {
    entity dragged;
    entity player;
    float value;    // slider value passed through event
} SlideEventData;

typedef struct {
    entity e;       // element toggled
    entity player;  // player toggling
    byte value;     // new value of toggle
} ToggleEventData;

zoxc_function(ClickEvent, void, ecs*, ClickEventData);
zoxc_function(SlideEvent, void, ecs*, const SlideEventData*);
zoxc_function(ToggleEvent, void, ecs*, const ToggleEventData*);

void zox_components_interaction(ecs* world) {
    // Selecting
    zoxd_tag(Selectable);
    zoxd_tag(SelectedBrighter);
    zoxd_byte(SelectState);
    zoxd_color(SelectedFillColor);
    zoxd_color(SelectedOutlineColor);
    zoxd_tag(ElementRaycaster);
    // Click
    zoxd_tag(Clickable);
    zoxd_tag(ClickMakeSound);
    zoxd_byte(ClickState);
    zoxd_byte(ClickType);
    zoxd_entity(ClickingEntity);
    zoxd_entity(Clicker);
    zoxd_byte(ClickDisabled);
    // Active
    zoxd_byte(ActiveState);
    zoxd_state(ActiveStateDirty);
    zoxd_color(ActiveColor);
    zoxd_entity(ActiveLink);
    zoxd_tag(ActiveSingle);
    // Dragging
    zoxd_tag(Dragable);
    zoxd_int2(DraggingDelta);
    zoxd_entity(DraggerLink);
    zoxd_entity(DraggedLink);
    zoxd_byte(DraggableState);
    // Slides
    zoxd(ClickEvent);
    zoxd(SlideEvent);
    zoxd(ToggleEvent);
    // Events
    zoxd_tag_event(TriggerSelect);
    zoxd_tag_event(Select);
}
