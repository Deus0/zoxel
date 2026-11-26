#include "drag.c"
#include "bar.c"
#include "scrolling.c"
#include "slide_event.c"
#include "toggle_event.c"
#include "toggle_state.c"
#include "scrollbar_handle.c"
#include "list_render_dirty.c"
#include "input.c"

void zox_systems_elements2(ecs *world) {
    // main thread (with the others in zoxp_spawn): it sets texts and the app's text input
    zox_system_1(
        input_field_system,
        zoxp_spawn,
        [in] inputs.Keyboard,
    );
    zox_system(
        element_drag_system,
        zoxp_update,
        [in] interactions.DraggableState,
        [in] interactions.DraggingDelta,
        // [in] interactions.DraggedLink
    );
    zox_system(
        scrollbar_system,
        zoxp_update,
        [in] interactions.DraggableState,
        [in] layouts.LayoutPosition,
        [in] layouts.LayoutSize,
        [none] elements2.ScrollbarHandle
    );
    zox_filter(
        scrollview_query,
        [in] transforms2.Position2,
        [in] layouts.LayoutSize,
        [none] elements2.Scrollview,
    );
    zox_system_ctx(
        scrollview_wheel_system,
        zoxp_update,
        scrollview_query,
        [in] inputs.ZeviceWheel,
        [in] inputs.ZevicePointerPosition,
        [in] core.Target,
        [none] !core.Disabled,
    );
    // make elements visible/invisible within list
    zox_system(
        list_render_dirty_system,
        zoxp_update,
        [in] containers.ListStart,
        [in] containers.ListVisible,
        [none] containers.ListPositionUpdate,
    );
    // resize handle when list dirty
    zox_on_add(
        scrollbar_handle_event,
        [in] containers.ListVisible,
        [in] containers.ListStart,
        [none] containers.ListDirty,
    );
    zox_system(
        elementbar_system,
        zoxp_update,
        [in] layouts.LayoutSize,
        [in] ui.BarLevel,
        [none] Elementbar
    );
    zox_system(
        slide_event_system,
        zoxp_update,
        [in] interactions.SlideEvent,
        [in] elements2.SlideBounds,
        [in] interactions.DraggableState,
        [in] layouts.LayoutConstraints,
        [in] layouts.LayoutPosition
    );
    // Main Thread needed due to nature of options
    zox_on_add(
        toggle_event_event,
        //[in] interactions.ClickState,
        [in] interactions.Clicker,
        [in] interactions.ToggleEvent,
        [out] interactions.ActiveState,
        [none] interactions.Click,
        [none] elements2.Toggle
    );
    /*zox_system(
        toggle_state_system,
        zoxp_update,
        [in] elements2.ToggleState,
        [in] elements2.ToggleStateDirty,
        [in] ui.ElementOutlineColor,
        [out] textures.OutlineColor,
        // [out] rendering.Brightness,
        [out] textures.GenerateTexture,
        [none] Toggle
    );*/
}
