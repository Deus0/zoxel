#include "drag.c"
#include "bar.c"
#include "scrolling.c"
#include "slide_event.c"
#include "list_render_dirty.c"
#include "toggle_event.c"
#include "toggle_state.c"
#include "scrollbar_handle.c"

void define_systems_elements2(ecs *world) {
    zox_system(
        ElementDragSystem,
        zoxp_update,
        [in] interaction.DraggableState,
        [in] interaction.DraggingDelta,
        [in] interaction.DraggedLink
    );
    zox_system(
        ScrollbarSystem,
        zoxp_update,
        [in] interaction.DraggableState,
        [in] layouts.LayoutPosition,
        [in] layouts.LayoutSize,
        [in] elements2.ScrollviewLink,
        [none] elements2.ScrollbarHandle
    );
    // make elements visible/invisible within list
    zox_system(
        ListRenderDirtySystem,
        zoxp_update,
        [in] containers.ListPositionDirty,
        [in] containers.ListStart,
        [in] containers.ListVisible
    );
    // resize handle when list dirty
    zox_system(
        ScrollbarHandleSystem,
        zoxp_update,
        [in] containers.ListDirty,
        [in] containers.ListVisible,
        [in] elements2.ScrollviewLink
    );
    zox_system(
        ElementbarSystem,
        zoxp_update,
        [in] layouts.LayoutSize,
        [in] ui.BarLevel,
        [none] Elementbar
    );
    zox_system(
        SlideEventSystem,
        zoxp_update,
        [in] interaction.SlideEvent,
        [in] elements2.SlideBounds,
        [in] interaction.DraggableState,
        [in] interaction.DraggerLink,
        [in] layouts.LayoutConstraints,
        [in] layouts.LayoutPosition
    );
    // Main Thread needed due to nature of options
    zox_system_1(
        ToggleEventSystem,
        zoxp_mainthread,
        [in] interaction.ClickState,
        [in] interaction.Clicker,
        [in] elements2.ToggleEvent,
        [out] interaction.ActiveState,
        [out] interaction.ActiveStateDirty,
        [none] elements2.Toggle
    );
    /*zox_system(
        ToggleStateSystem,
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
