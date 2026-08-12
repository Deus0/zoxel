#include "click_event.c"
#include "mouse_element.c"
#include "click_zevice.c"
#include "click_device.c"
#include "keyboard_click.c"
#include "raycast.c"
#include "selected.c"
#include "click_sound.c"
#include "drag.c"
#include "active.c"
#include "link.c"
#include "resulter.c"
zox_increment_system_with_reset_extra(ClickState, zox_click_state_trigger_clicked, zox_click_state_clicked_idle, zox_click_state_trigger_released, zox_click_state_idle);
zox_increment_system_with_reset_extra(SelectState, zox_state_select_trigger, zox_state_select_idle, zox_state_deselect_trigger, zox_state_deselect_idle);

void zox_define_systems_interaction(ecs* world) {
    zoxd_system_increment(ClickState);
    zoxd_system_increment(SelectState);
    zox_filter(
        raycast_query,
        [in] transforms2.Position2,
        [in] layouts.LayoutSize,   // use Size2
        [in] layouts.Layer2D,
        [in] rendering.RenderDisabled,
        [none] elements.Element,
        [none] interaction.Selectable
    );
    zox_system_ctx(
        ElementRaycastSystem,
        zoxp_update,
        raycast_query,
        [in] inputs.ZeviceDisabled,
        [in] inputs.ZevicePointerPosition,
        [out] raycasts.RaycasterTarget,
    );
    zox_system(
        ZeviceClickSystem,
        zoxp_update,
        [in] inputs.ZeviceDisabled,
        [in] inputs.DeviceLink,
        [in] raycasts.RaycasterTarget,
        [out] interaction.ClickingEntity,
        [none] inputs.Zevice
    );
    zox_system(
        KeyboardClickSystem,
        zoxp_update,
        [in] inputs.DeviceDisabled,
        [in] players.PlayerLink,
        [in] raycasts.RaycasterTarget,
        [in] inputs.Keyboard,
        [out] interaction.ClickingEntity,
        [none] inputs.Device
    );
    // Coloring Animations
    zox_system(
        ElementSelectedFillColorSystem,
        zoxp_update,
        [in] interaction.SelectState,
        [in] elements.ElementFillColor,
        [in] interaction.SelectedFillColor,
        [out] textures.FillColor,
        [out] textures.GenerateTexture,
        [none] elements.Element
    );
    zox_system(
        ElementSelectedOutlineColorSystem,
        zoxp_update,
        [in] interaction.SelectState,
        [in] elements.ElementOutlineColor,
        [in] interaction.SelectedOutlineColor,
        [out] textures.OutlineColor,
        [out] textures.GenerateTexture,
        [none] elements.Element
    );
    // Active
    zox_system(
        ElementActiveSystem,
        zoxp_update,
        [in] interaction.ActiveState,
        [in] interaction.ActiveStateDirty,
        [in] elements.ElementOutlineColor,
        [in] interaction.ActiveColor,
        [out] textures.OutlineColor,
        [out] textures.GenerateTexture,
        [none] elements.Element
    );
    zox_system(
        ElementSelectedBrighterSystem,
        zoxp_update,
        [in] interaction.SelectState,
        [out] rendering.Brightness,
        [none] elements.Element,
        [none] interaction.SelectedBrighter,
        // [none] !interaction.SelectedFillColor
    );
    zox_system(
        DraggerEndSystem,
        zoxp_update,
        [out] interaction.DraggableState,
        [out] interaction.DraggerLink,
        [out] interaction.DraggingDelta
    );
    zox_system(
        ElementActiveLinkSystem,
        zoxp_update,
        [in] interaction.ActiveState,
        [in] interaction.ActiveStateDirty,
        [none] elements.Element
    );
    zox_system(
        MouseElementSystem,
        zoxp_update,
        [in] inputs.ZeviceLink,
        [in] layouts.Anchor,
        [out] layouts.LayoutPosition,
        [out] layouts.LayoutPositionDirty,
        [none] elements.MouseElement
    );
    zox_system(
        RaycasterResulterSystem,
        zoxp_update,
        [in] inputs.DeviceLink,
        [in] raycasts.RaycasterTarget,
        [out] raycasts.RaycasterResult,
        [none] inputs.Zevice
    );
    zox_system_1(
        ClickSoundSystem,
        zoxp_mainthread,
        [in] interaction.ClickState,
        [none] interaction.ClickMakeSound
    );
    zox_system_1(
        ButtonClickEventSystem,
        zoxp_mainthread,
        [in] interaction.ClickEvent,
        [in] interaction.ClickState,
        [out] interaction.Clicker,
        [none] elements.Element
    );
}


    // NOTE: Has to be after raycasting system
    /*zox_system(
        DeviceClickSystem,
        zoxp_update,
        [in] inputs.DeviceDisabled,
        [in] raycasts.RaycasterTarget,
        [out] interaction.ClickingEntity,
        [none] inputs.Device
    );*/
