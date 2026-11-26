static entity zox_tst_mask_button(ecs* world, entity parent, int2 position, int2 size, color fill) {
    const entity e = spawn_uic(world, prefab_button, parent, float2_half, position, size, size, fill, fill);
    zox_setv(e, ElementFillColor, fill);
    zox_setv(e, ElementOutlineColor, fill);
    zox_setv(e, SelectedFillColor, fill);
    zox_setv(e, SelectedOutlineColor, fill);
    return e;
}

static int4 zox_tst_mask_bounds(ecs* world, const CanvasViewport* viewport, entity e) {
    return get_element_pointer_bounds(world, viewport, e, zox_getv(e, Position2), zox_getv(e, LayoutSize));
}

static byte zox_tst_ui_mask_ready(ecs* world) {
    static entity canvas, mask, wide, nested, nested_child, underneath;
    static int2 inside, outside;
    static int step;
    zox_tst_progress(step);
    switch (step) {
        case 0: {
            zox_tst_waiting = "the mask fixture canvas";
            canvas = zox_tst_ui_fixture_canvas(world);
            if (!canvas) {
                return 0;
            }
            underneath = zox_tst_mask_button(world, canvas, (int2) { 100, 0 },
                (int2) { 96, 48 }, zox_tst_ui_green);
            mask = spawn_ui(world, prefab_layout2, canvas, float2_half, int2_zero,
                (int2) { 128, 80 }, int2_zero);
            zox_setv(underneath, Layer, 32);
            zox_add(mask, Mask);
            wide = zox_tst_mask_button(world, mask, int2_zero, (int2) { 256, 48 }, zox_tst_ui_red);
            zox_setv(wide, Layer, 33);
            nested = spawn_ui(world, prefab_layout2, mask, float2_half, (int2) { 44, 16 },
                (int2) { 64, 36 }, int2_zero);
            zox_add(nested, Mask);
            nested_child = spawn_ui(world, prefab_layout2, nested, float2_half, int2_zero,
                (int2) { 128, 48 }, int2_zero);
            break;
        }
        case 1: {
            zox_tst_waiting = "the masks to lay out their descendants";
            CanvasViewport viewport;
            if (!get_canvas_viewport(world, canvas, &viewport) || !get_ui_clip(world, nested_child) ||
                !get_ui_clip(world, wide) || !zox_getv(nested, Position2).x ||
                !zox_getv(wide, TextureGPULink) || !zox_getv(underneath, TextureGPULink)) {
                return 0;
            }
            const int4 bounds = zox_tst_mask_bounds(world, &viewport, mask);
            const int2 origin = viewport.position;
            const int4 outer = { bounds.x - origin.x, bounds.y - origin.x,
                bounds.z - origin.y, bounds.w - origin.y };
            const int4 expected = { outer.x + 76, outer.y, outer.z + 38, outer.z + 74 };
            zox_tst_check(!get_ui_clip(world, mask) && int4_equals(*get_ui_clip(world, wide), outer),
                "the plain mask did not clip its wide child to its own rectangle");
            zox_tst_check(int4_equals(*get_ui_clip(world, nested_child), expected),
                "the nested mask did not narrow the ancestor clip");
            inside = (int2) { bounds.x + 12, (bounds.z + bounds.w) / 2 };
            outside = (int2) { bounds.y + 8, inside.y };
            zox_tst_ui_capture("ui_mask.ppm");
            break;
        }
        case 2: {
            zox_tst_waiting = "the clipped frame";
            if (zox_screenshot_request[0]) {
                return 0;
            }
            int2 size = int2_zero;
            byte* pixels = zox_tst_load_ppm(zox_tst_path("ui_mask.ppm"), &size);
            zox_tst_check(zox_tst_ui_pixel_is(pixels, size, inside, zox_tst_ui_red),
                "the wide red child did not draw inside the mask");
            zox_tst_check(zox_tst_ui_pixel_is(pixels, size, outside, zox_tst_ui_green),
                "the hidden part of the wide child covered the green button");
            free(pixels);
            zox_tst_mouse(outside, 0);
            break;
        }
        case 3:
            zox_tst_waiting = "the pointer to reach the button beneath the clipped child";
            if (zox_tst_mouse_target(world, dbg_player) != underneath) {
                return 0;
            }
            zox_tst_mouse(inside, 0);
            break;
        default:
            zox_tst_waiting = "the visible part of the wide child to receive the pointer";
            return zox_tst_mouse_target(world, dbg_player) == wide;
    }
    step++;
    return 0;
}
