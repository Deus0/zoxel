#include "drag.c"
#include "bar.c"
#include "scrolling.c"
#include "tooltip.c"
#include "slide_event.c"
#include "list_render_dirty.c"
#include "toggle_event.c"
#include "toggle_state.c"
#include "scrollbar_handle.c"

void define_systems_elements2(ecs *world) {

    zox_system(
        ElementDragSystem,
        EcsPostLoad,
        [in] elements.DraggableState,
        [in] elements.DraggingDelta,
        [in] elements.DraggedLink
    );

    zox_system(
        ScrollbarSystem,
        EcsPostUpdate,
        [in] elements.DraggableState,
        [in] layouts2.LayoutPosition,
        [in] layouts2.LayoutSize,
        [in] hierarchys.ParentLink,
        [in] elements2.ScrollviewLink,
        [none] elements2.ScrollbarHandle
    );

    // make elements visible/invisible within list
    zox_system(
        ListRenderDirtySystem,
        EcsOnUpdate,
        [in] layouts2.ListPositionDirty,
        [in] layouts2.ListStart,
        [in] layouts2.ListVisible,
        [in] hierarchys.Children
    );

    // resize handle when list dirty
    zox_system(
        ScrollbarHandleSystem,
        EcsOnUpdate,
        [in] layouts2.ListDirty,
        [in] layouts2.ListVisible,
        [in] elements2.ScrollviewLink
    );

    zox_system(
        Elementbar2System,
        EcsPostUpdate,
        [in] elements.ElementBar,
        [in] elements.ElementBarSize,
        [in] hierarchys.Children,
        [in] layouts2.LayoutSize,
        [none] Elementbar2
    );
    zox_system(
        TooltipSystem,
        EcsOnUpdate,
        [in] elements.SelectState,
        [in] layouts2.CanvasLink,
        [in] elements.TooltipEvent
    );
    zox_system(
        SlideEventSystem,
        EcsOnUpdate,
        [in] elements.SlideEvent,
        [in] elements2.SlideBounds,
        [in] elements.DraggableState,
        [in] elements.DraggerLink,
        [in] layouts2.LayoutConstraints,
        [in] layouts2.LayoutPosition
    );
    // Main Thread needed due to nature of options
    zox_system_1(
        ToggleEventSystem,
        EcsOnUpdate,
        [in] elements.ClickState,
        [in] elements.Clicker,
        [in] elements2.ToggleEvent,
        [out] elements2.ToggleState,
        [out] elements2.ToggleStateDirty,
        [none] Toggle
    );
    zox_system(
        ToggleStateSystem,
        EcsOnUpdate,
        [in] elements2.ToggleState,
        [in] elements2.ToggleStateDirty,
        [out] textures.OutlineColor,
        [out] rendering.Brightness,
        [out] textures.GenerateTexture,
        [none] Toggle
    );

}
