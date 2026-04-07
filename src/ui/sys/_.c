#include "raycast.c"
#include "selected.c"
#include "click_sound.c"
#include "texture_size.c"
#include "texture_size_generate.c"
#include "mesh.c"

#include "layouts2D/canvas_resize.c"
#include "layouts2D/canvas_stack.c"
#include "layouts2D/window_layer.c"

#include "rendering/element_begin.c"
#include "rendering/texture_dirty_begin.c"
#include "rendering/texture_gpu_begin.c"
#include "rendering/element_renderer.c"

#include "inputs/button_click_event.c"
#include "inputs/mouse_element.c"
#include "inputs/device_click.c"
#include "inputs/zevice_click.c"
#include "inputs/keyboard_click.c"

#include "active/animate.c"
#include "active/link.c"

#include "drag/dragger_end.c"

// zox_increment_system_with_reset(InitializeElement, zox_dirty_end);
// zox_increment_system_with_reset(ActiveStateDirty, zox_dirty_end);
zox_increment_system_with_reset_extra(ClickState, zox_click_state_trigger_clicked, zox_click_state_clicked_idle, zox_click_state_trigger_released, zox_click_state_idle);
zox_increment_system_with_reset_extra(SelectState, zox_select_state_trigger_selected, zox_select_state_selected, zox_select_state_trigger_deselect, zox_select_state_deselected_idle);

void define_systems_elements(ecs *world) {
    zoxd_system_increment(ClickState);
    zoxd_system_increment(SelectState);
    zox_filter(
        raycast_query,
        [in] layouts2.CanvasPosition,
        [in] layouts2.LayoutSize,
        [in] layouts2.Layer2D,
        [in] rendering.RenderDisabled,
        [none] Element,
        [none] Selectable
    );
    zox_system_ctx(
        ElementRaycastSystem,
        EcsOnUpdate,
        raycast_query,
        [in] raycasts.Raycaster,
        [in] inputs.DeviceLink,
        [out] raycasts.RaycasterTarget,
        [out] WindowRaycasted
    );

    // inputs
    zox_system(
        ZeviceClickSystem,
        EcsPostUpdate,
        [in] inputs.DeviceLink,
        [in] raycasts.RaycasterTarget,
        [in] WindowRaycasted,
        [out] raycasts.RaycasterResult,
        [out] ClickingEntity,
        [out] WindowTarget,
        [none] inputs.Zevice
    );
    zox_system(
        DeviceClickSystem,
        EcsPostUpdate,
        [in] inputs.DeviceDisabled,
        [in] players.PlayerLink,
        [in] raycasts.RaycasterTarget,
        [in] WindowRaycasted,
        [in] hierarchys.Children,
        [out] ClickingEntity,
        [out] WindowTarget,
        [none] inputs.Device
    );
    zox_system(
        KeyboardClickSystem,
        EcsPostUpdate,
        [in] inputs.DeviceDisabled,
        [in] players.PlayerLink,
        [in] raycasts.RaycasterTarget,
        [in] WindowRaycasted,
        [in] inputs.Keyboard,
        [out] ClickingEntity,
        [out] WindowTarget,
        [none] inputs.Device
    );
    zox_system(
        CanvasStackSystem,
        EcsOnLoad,
        [in] hierarchys.Children,
        [out] layouts2.WindowToTop,
        [out] WindowsLayers,
        [out] WindowsCount,
        [none] layouts2.Canvas
    );
    zox_system(
        WindowLayerSystem,
        EcsOnLoad,
        [in] SetWindowLayer,
        [in] layouts2.CanvasLink,
        [in] hierarchys.Children,
        [out] WindowLayer,
        [out] layouts2.Layer2D,
        [none] Window
    );

    zox_system(
        ElementSelectedSystem,
        EcsOnUpdate,
        [in] elements.SelectState,
        [out] rendering.Brightness,
        [none] Element
    );

    // Active
    zox_system(
        ElementActiveSystem,
        EcsOnUpdate,
        [in] elements.ActiveState,
        [in] elements.ActiveStateDirty,
        [in] elements.ElementColor,
        [in] elements.ActiveColor,
        [out] textures.OutlineColor,
        [out] rendering.Brightness,
        [out] textures.GenerateTexture,
        [none] Element
    );
    zox_system(
        ElementActiveLinkSystem,
        EcsOnUpdate,
        [in] elements.ActiveState,
        [in] elements.ActiveStateDirty,
        [in] hierarchys.ParentLink,
        [none] Element
    );

    zox_system(
        MouseElementSystem,
        zox_transforms_stage,
        [in] inputs.ZeviceLink,
        [in] layouts2.Anchor,
        [in] layouts2.CanvasLink,
        [out] layouts2.LayoutPosition,
        [out] layouts2.LayoutPositionDirty,
        [none] MouseElement
    );
    zox_system(
        DraggerEndSystem,
        EcsPostLoad,
        [out] elements.DraggableState,
        [out] DraggerLink,
        [out] elements.DraggingDelta
    );
    if (!headless) {
        zox_system(
            CanvasResizeSystem,
            EcsOnUpdate,
            [in] cameras.ScreenToCanvas,
            [in] apps.AppLink,
            [in] hierarchys.Children,
            [out] layouts2.LayoutPosition,
            [out] layouts2.LayoutSize,
            [out] layouts2.LayoutPositionDirty,
            [out] layouts2.LayoutSizeDirty,
            [none] layouts2.Canvas
        );
    }
    // all ui
    zox_render2D_system(
        ElementRenderSystem,
        [in] transforms2.Position2,
        [in] transforms2.Rotation2D,
        [in] transforms.Scale1D,
        [in] layouts2.Layer2D,
        [in] rendering.RenderDisabled,
        [in] rendering.Brightness,
        [in] rendering.Alpha,
        [in] rendering.MeshGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.TextureGPULink,
        [none] ElementRender,
        [none] !cameras.RenderTexture
    );
    add_system_process_counter(world, zox_id(ElementRenderSystem));

    // healthbars
    zox_system_1(
        ClickSoundSystem,
        zoxp_mainthread,
        [in] elements.ClickState,
        [none] ClickMakeSound
    );
    zox_system_1(
        ButtonClickEventSystem,
        zoxp_mainthread,
        [in] ClickEvent,
        [in] elements.ClickState,
        [out] Clicker,
        [none] Element
    );

    if (!headless) {
        // EcsOnLoad - zoxp_mainthread
        zox_system_1(
            ElementBeginSystem,
            EcsPostLoad,
            [in] elements.InitializeElement,
            [in] layouts2.LayoutSize,
            [in] rendering.MeshAlignment,
            [in] layouts2.CanvasLink,
            [out] rendering.MeshDirty,
            [out] rendering.MeshVertices2D,
            [out] rendering.MeshGPULink,
            [out] rendering.UvsGPULink,
            [none] Element
        );
        zox_system_1(
            TextureDirtyBeginSystem,
            EcsPostLoad,
            [in] elements.InitializeElement,
            [out] rendering.TextureDirty
        );
        zox_system_1(
            TextureGpuBeginSystem,
            EcsPostLoad,
            [in] elements.InitializeElement,
            [out] rendering.TextureGPULink
        );
    }

    zox_system(
        TextureSizeSystem,
        EcsPreUpdate,
        [in] layouts2.LayoutSizeDirty,
        [in] layouts2.LayoutSize,
        [out] rendering.TextureSize,
        [none] textures.FixToLayout
    );
    zox_system(
        TextureSizeGenerateSystem,
        EcsPreUpdate,
        [in] layouts2.LayoutSizeDirty,
        [out] textures.GenerateTexture
    );
    zox_system(
        LayoutMeshSystem,
        EcsPostUpdate,
        [in] layouts2.LayoutSizeDirty,
        [in] layouts2.CanvasLink,
        [in] layouts2.LayoutSize,
        [in] rendering.MeshAlignment,
        [out] rendering.MeshVertices2D,
        [out] rendering.MeshDirty
    );

}
