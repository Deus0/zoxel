// Helpers for ui tests (used by huds/tst and later ui tests)
//  - find a button by its click event, and click it directly (tests what the button does)
//  - or click it with the fake mouse (tests the whole path: mouse > raycast > click event)
//      - the fake mouse replaces the os mouse while set, see zox_mouse_fake in inputs/set

// finds e, or a descendant of e, with a ClickEvent of on_click
entity zox_tst_find_click(
    ecs* world,
    entity e,
    void (*on_click)(ecs*, ClickEventData))
{
    if (zox_has(e, ClickEvent) && zox_getv(e, ClickEvent) == on_click) {
        return e;
    }
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity found = zox_tst_find_click(world, it2.entities[j], on_click);
            if (found) {
                return found;
            }
        }
    }
    return 0;
}

// calls a button's click event, as if player clicked it
static inline void zox_tst_click(
    ecs* world,
    entity player,
    entity button)
{
    void (*on_click)(ecs*, ClickEventData) = zox_getv(button, ClickEvent);
    on_click(world, (ClickEventData) { .clicker = player, .clicked = button });
}

// counts every descendant of e (not e itself)
int zox_tst_count_descendants(
    ecs* world,
    entity e)
{
    int count = 0;
    iter it2 = zox_children(world, e);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            count += 1 + zox_tst_count_descendants(world, it2.entities[j]);
        }
    }
    return count;
}

// is e the ancestor, or one of its descendants
byte zox_tst_is_within(
    ecs* world,
    entity e,
    entity ancestor)
{
    for (int depth = 0; zox_valid(e) && depth < 64; depth++) {
        if (e == ancestor) {
            return 1;
        }
        e = zox_get_parent(world, e);
    }
    return 0;
}

// a ui element's centre in pointer space (bottom left origin), as ElementRaycastSystem sees it
//  - returns 0 if the element or the canvas's camera isn't ready
byte zox_tst_ui_pointer_position(
    ecs* world,
    entity canvas,
    entity e,
    int2* position)
{
    const entity camera = zox_get_link(world, canvas, CameraLink);
    if (!zox_valid(camera) ||
        !zox_has(e, Position2) ||
        !zox_has(e, LayoutSize))
    {
        return 0;
    }
    const float2 canvas_sizef = int2_to_float2(zox_getv(canvas, LayoutSize));
    if (canvas_sizef.y <= 0) {
        return 0;
    }
    const float aspect_ratio = canvas_sizef.x / canvas_sizef.y;
    const int2 canvas_position = zox_getv(camera, ScreenPosition);
    const int2 canvas_size = zox_getv(camera, PixelSize);
    const float2 p = zox_getv(e, Position2);
    const int2 lsize = zox_getv(e, LayoutSize);
    const int2 viewport_position = {
        canvas_position.x + (int)((p.x / aspect_ratio + 0.5f) * canvas_size.x),
        canvas_position.y + (int)((p.y + 0.5f) * canvas_size.y)
    };
    int4 bounds = {
        viewport_position.x - lsize.x / 2,
        viewport_position.x + lsize.x / 2,
        viewport_position.y - lsize.y / 2,
        viewport_position.y + lsize.y / 2
    };
    if (zox_has(e, MeshAlignment)) {
        const float4 scaler = alignment_to_scaler(zox_getv(e, MeshAlignment));
        bounds = (int4) {
            viewport_position.x - lsize.x * scaler.x,
            viewport_position.x + lsize.x * scaler.y,
            viewport_position.y - lsize.y * scaler.z,
            viewport_position.y + lsize.y * scaler.w
        };
    }
    *position = (int2) {
        (bounds.x + bounds.y) / 2,
        (bounds.z + bounds.w) / 2
    };
    return 1;
}

// moves the fake mouse, and sets its left button, it replaces the os mouse until zox_tst_mouse_end
static inline void zox_tst_mouse(
    int2 position,
    byte left)
{
    zox_mouse_fake = 1;
    zox_mouse_fake_position = position;
    zox_mouse_fake_left = left;
    zox_mouse_fake_right = 0;
}

static inline void zox_tst_mouse_end() {
    zox_mouse_fake = 0;
    zox_mouse_fake_left = 0;
    zox_mouse_fake_right = 0;
}

// the player's mouse pointer zevice (the child that raycasts the ui)
entity zox_tst_mouse_pointer(
    ecs* world,
    entity player)
{
    const entity mouse = zox_get_child_by_id(world, player, zox_id(Mouse));
    if (!zox_valid(mouse)) {
        return 0;
    }
    iter it2 = zox_children(world, mouse);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity e = it2.entities[j];
            if (zox_has(e, ZevicePointerPosition) && zox_has(e, EntityTarget)) {
                return e;
            }
        }
    }
    return 0;
}

// the ui element the player's mouse is over, 0 if none
entity zox_tst_mouse_target(
    ecs* world,
    entity player)
{
    const entity pointer = zox_tst_mouse_pointer(world, player);
    return zox_valid(pointer) ? zox_getv(pointer, EntityTarget) : 0;
}

// the player's MouseLock, -1 if it has no mouse
int zox_tst_mouse_lock(
    ecs* world,
    entity player)
{
    const entity mouse = zox_get_child_by_id(world, player, zox_id(Mouse));
    return zox_valid(mouse) ? zox_getv(mouse, MouseLock) : -1;
}
