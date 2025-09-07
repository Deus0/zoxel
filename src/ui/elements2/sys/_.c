#include "drag.c"
#include "bar.c"
#include "scrolling.c"
#include "tooltip.c"
#include "slide_event.c"
#include "list_render_dirty.c"

void define_systems_elements2(ecs *world) {
    zox_system(
        ElementDragSystem,
        EcsPostLoad,
        [in] elements.core.DraggableState,
        [in] elements.core.DraggingDelta,
        [in] elements.core.DraggedLink
    );
    zox_system(
        ScrollbarSystem,
        EcsPostUpdate,
        [in] elements.core.DraggableState,
        [in] layouts2.LayoutPosition,
        [in] layouts2.LayoutSize,
        [in] hierarchys.ParentLink,
        [none] ScrollbarButton
    );
    zox_system(
        ListRenderDirtySystem,
        EcsPostUpdate,
        [in] layouts2.ListDirty,
        [in] layouts2.ListStart,
        [in] layouts2.ListVisible,
        [in] hierarchys.Children
    );
    zox_system(
        Elementbar2System,
        EcsPostUpdate,
        [in] elements.core.ElementBar,
        [in] elements.core.ElementBarSize,
        [in] hierarchys.Children,
        [in] layouts2.LayoutSize,
        [none] Elementbar2
    );
    zox_system(
        TooltipSystem,
        EcsOnUpdate,
        [in] elements.core.SelectState,
        [in] layouts2.CanvasLink,
        [in] elements.core.TooltipEvent
    );
    zox_system(
        SlideEventSystem,
        EcsOnUpdate,
        [in] elements.core.SlideEvent,
        [in] elements2.SlideBounds,
        [in] elements.core.DraggableState,
        [in] elements.core.DraggerLink,
        [in] layouts2.LayoutConstraints,
        [in] layouts2.LayoutPosition
    );
}