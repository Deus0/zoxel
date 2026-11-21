#include "click_event.c"
#include "mouse_element.c"
#include "click_zevice.c"
#include "keyboard_click.c"
#include "raycast.c"
#include "selected.c"
#include "click_sound.c"
#include "drag.c"
#include "active.c"
#include "link.c"
#include "resulter.c"
#include "state.c"

zox_increment_system_with_reset_extra(ClickState, zox_click_state_trigger_clicked, zox_click_state_clicked_idle, zox_click_state_trigger_released, zox_click_state_idle);
zox_increment_system_with_reset_extra(SelectState, zox_state_select_trigger, zox_state_select_idle, zox_state_deselect_trigger, zox_state_deselect_idle);

void zox_systems_interaction(ecs* world) {
    zoxd_system_increment(ClickState);
    zoxd_system_increment(SelectState);
    zox_filter(
        raycast_query,
        [in] transforms2.Position2,
        [in] layouts.LayoutSize,   // use Size2
        [in] layouts.Layer,
        [in] rendering.RenderDisabled,
        [none] ui.Element,
        [none] interactions.Selectable,
    );
    zox_system_ctx(
        element_raycast_system,
        zoxp_update,
        raycast_query,
        [in] inputs.ZevicePointerPosition,
        [out] core.Target,
        [none] !core.Disabled,
    );
    zox_system(
        zevice_click_system,
        zoxp_update,
        [in] core.Target,
        [out] interactions.ClickingEntity,
        [none] inputs.Zevice,
        [none] !core.Disabled,
    );
    zox_system(
        keyboard_click_system,
        zoxp_update,
        [in] core.Target,
        [in] inputs.Keyboard,
        [out] interactions.ClickingEntity,
        [none] inputs.Device,
        [none] !core.Disabled,
    );
    // Coloring Animations
    zox_system(
        element_selected_fill_color_system,
        zoxp_update,
        [in] interactions.SelectState,
        [in] ui.ElementFillColor,
        [in] interactions.SelectedFillColor,
        [out] textures.FillColor,
        [out] textures.GenerateTexture,
        [none] ui.Element
    );
    zox_system(
        element_selected_outline_color_system,
        zoxp_update,
        [in] interactions.SelectState,
        [in] ui.ElementOutlineColor,
        [in] interactions.SelectedOutlineColor,
        [out] textures.OutlineColor,
        [out] textures.GenerateTexture,
        [none] ui.Element
    );
    // Active
    zox_on_add(
        element_active_event,
        [in] interactions.ActiveState,
        [in] ui.ElementOutlineColor,
        [in] interactions.ActiveColor,
        [out] textures.OutlineColor,
        [out] textures.GenerateTexture,
        [none] ui.Element,
        [none] interactions.ActiveStateDirty
    );
    zox_system(
        element_selected_brighter_system,
        zoxp_update,
        [in] interactions.SelectState,
        [out] rendering.Brightness,
        [none] ui.Element,
        [none] interactions.SelectedBrighter,
    );
    zox_system(
        dragger_end_system,
        zoxp_update,
        [out] interactions.DraggableState,
        [out] interactions.DraggingDelta
    );
    zox_on_add(
        element_active_link_event,
        [in] interactions.ActiveState,
        [none] ui.Element,
        [none] interactions.ActiveStateDirty
    );
    zox_system(
        mouse_element_system,
        zoxp_update,
        [in] layouts.Anchor,
        [out] layouts.LayoutPosition,
        [none] ui.MouseElement
    );
    zox_on_add(
        click_sound_event,
        // [in] interactions.ClickState,
        [none] interactions.Click,
        [none] interactions.ClickMakeSound
    );
    zox_on_add(
        button_click_event_event,
        [in] interactions.ClickEvent,
        [out] interactions.Clicker,
        [none] interactions.Click,
        [none] ui.Element,
    );

    zox_system(
        trigger_select_system,
        zoxp_remove,
        [none] interactions.TriggerSelect,
    );
    zox_system(
        select_system,
        zoxp_remove,
        [none] interactions.Select,
    );

    // the ui events' cleanups
    zox_on_add(deselect_cleanup_event, [none] interactions.Deselect);
    zox_on_add(click_cleanup_event, [none] interactions.Click);
    zox_on_add(right_click_cleanup_event, [none] interactions.RightClick);
    zox_on_add(press_cleanup_event, [none] interactions.Press);
}
