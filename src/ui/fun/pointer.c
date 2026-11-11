// Maps ui elements into pointer space (pixels, bottom left origin, like ZevicePointerPosition)
//  - used by ElementRaycastSystem, scrollview wheel scrolling and ui tests

// where a canvas is drawn on screen
typedef struct {
    int2 position;
    int2 size;
    float aspect_ratio;
} CanvasViewport;

// returns 0 if the canvas or its camera isn't ready
byte get_canvas_viewport(
    ecs* world,
    entity canvas,
    CanvasViewport* viewport)
{
    if (!zox_valid(canvas)) {
        return 0;
    }
    const entity camera = zox_get_link(world, canvas, CameraLink);
    if (!zox_valid(camera)) {
        return 0;
    }
    const float2 canvas_size = int2_to_float2(zox_getv(canvas, LayoutSize));
    if (canvas_size.y <= 0) {
        return 0;
    }
    viewport->aspect_ratio = canvas_size.x / canvas_size.y;
    viewport->position = zox_getv(camera, ScreenPosition);
    viewport->size = zox_getv(camera, PixelSize);
    return 1;
}

static inline byte is_in_canvas_viewport(
    const CanvasViewport* viewport,
    int2 point)
{
    return point.x >= viewport->position.x &&
        point.x <= viewport->position.x + viewport->size.x &&
        point.y >= viewport->position.y &&
        point.y <= viewport->position.y + viewport->size.y;
}

// an element's bounds in pointer space: x min, x max, y min, y max
int4 get_element_pointer_bounds(
    ecs* world,
    const CanvasViewport* viewport,
    entity e,
    float2 position,
    int2 size)
{
    const int2 centre = {
        viewport->position.x + (int)((position.x / viewport->aspect_ratio + 0.5f) * viewport->size.x),
        viewport->position.y + (int)((position.y + 0.5f) * viewport->size.y)
    };
    if (zox_has(e, MeshAlignment)) {
        const float4 scaler = alignment_to_scaler(zox_getv(e, MeshAlignment));
        return (int4) {
            centre.x - size.x * scaler.x,
            centre.x + size.x * scaler.y,
            centre.y - size.y * scaler.z,
            centre.y + size.y * scaler.w
        };
    }
    return (int4) {
        centre.x - size.x / 2,
        centre.x + size.x / 2,
        centre.y - size.y / 2,
        centre.y + size.y / 2
    };
}

static inline byte is_in_pointer_bounds(
    int4 bounds,
    int2 point)
{
    return point.x >= bounds.x &&
        point.x <= bounds.y &&
        point.y >= bounds.z &&
        point.y <= bounds.w;
}
