// tst: taskbar_lifecycle, independently registered entries draw, open and track their own windows.
static entity zox_tst_taskbar_ids[2];
static entity zox_tst_taskbar_windows[2];
static const color zox_tst_taskbar_colors[] = { { 255, 0, 0, 255 }, { 0, 255, 0, 255 } };
static char* const zox_tst_taskbar_names[] = { "Fixture Alpha", "Fixture Beta" };
static char* const zox_tst_taskbar_textures[] = { "tst_taskbar_alpha", "tst_taskbar_beta" };

static entity zox_tst_taskbar_open(ecs* world, entity player, int index) {
    const entity canvas = zox_get_link(world, player, CanvasLink);
    const int2 position = { index ? 100 : -100, 0 };
    const entity window = spawn_window(world, prefab_window, prefab_body, zox_tst_taskbar_names[index], canvas,
        position, (int2) { 180, 72 }, float2_half, 12, byte2_single(4), on_closed_taskbar_window).x;
    zox_add_id(window, zox_tst_taskbar_ids[index]);
    return window;
}

static entity zox_tst_taskbar_open_alpha(ecs* world, entity player) {
    return zox_tst_taskbar_open(world, player, 0);
}

static entity zox_tst_taskbar_open_beta(ecs* world, entity player) {
    return zox_tst_taskbar_open(world, player, 1);
}

static entity zox_tst_taskbar_spawn(ecs* world, entity canvas) {
    const entity previous_manager = taskbar_manager;
    spawn_taskbar_manager(world);
    const entity_fun spawners[] = { zox_tst_taskbar_open_alpha, zox_tst_taskbar_open_beta };
    for (int i = 0; i < 2; i++) {
        zox_tst_taskbar_ids[i] = zox_new();
        const entity texture = zox_new();
        TextureData pixels = { 0 };
        initialize_TextureData(&pixels, 16 * 16);
        for (int j = 0; j < pixels.length; j++) {
            pixels.value[j] = zox_tst_taskbar_colors[i];
        }
        zox_set_ptr(texture, TextureData, pixels);
        zox_setv(texture, TextureSize, ((int2) { 16, 16 }));
        string_hashmap_add(files_hashmap_textures, new_string_data_clone(zox_tst_taskbar_textures[i]), texture);
        add_taskbar_button(world, (TaskbarData) {
            .index = i,
            .component_id = zox_tst_taskbar_ids[i],
            .spawn = spawners[i],
            .texture_name = zox_tst_taskbar_textures[i],
            .tooltip_text = zox_tst_taskbar_names[i]
        });
    }
    const entity taskbar = spawn_taskbar(world, canvas);
    taskbar_manager = previous_manager;
    return taskbar;
}

static byte zox_tst_taskbar_ready(ecs* world) {
    static entity canvas, taskbar, frames[2], icons[2];
    static int step, action, click_step;
    static int2 icon_points[2], point;
    // Open both, then close and reopen each through its own controls.
    static const int entries[] = { 0, 1, 0, 0, 1, 1 };
    const int index = entries[action];
    const byte closing = action == 2 || action == 4;
    zox_tst_progress(action * 10 + step);
    switch (step) {
        case 0: {
            zox_tst_waiting = "the UI fixture canvas";
            canvas = zox_tst_ui_fixture_canvas(world);
            if (!canvas) {
                return 0;
            }
            taskbar = zox_tst_taskbar_spawn(world, canvas);
            const entity body = zox_get_child_by_id(world, taskbar, zox_id(WindowBody));
            zox_tst_check(zox_valid(body), "taskbar has no body");
            if (!zox_valid(body)) {
                return 1;
            }
            entity children[8];
            const int count = zox_get_children_by_id(world, body, children, 8, zox_id(TaskbarWindowID));
            zox_tst_check(count == 2, "taskbar spawned [%i] frames for two registered entries", count);
            for (int i = 0; i < count; i++) {
                const entity id = zox_getv(children[i], TaskbarWindowID);
                for (int j = 0; j < 2; j++) {
                    if (id == zox_tst_taskbar_ids[j]) {
                        zox_tst_check(!frames[j], "duplicate frame for %s", zox_tst_taskbar_names[j]);
                        frames[j] = children[i];
                        icons[j] = zox_get_child_by_id(world, frames[j], zox_id(Icon));
                    }
                }
            }
            for (int i = 0; i < 2; i++) {
                zox_tst_check(zox_valid(icons[i]), "no taskbar icon for %s", zox_tst_taskbar_names[i]);
                if (!zox_valid(icons[i])) {
                    return 1;
                }
            }
            break;
        }
        case 1:
            zox_tst_waiting = "the taskbar to finish opening";
            if (zox_getv(taskbar, LocalScale1) != 1 ||
                !zox_tst_ui_pointer_position(world, canvas, icons[0], &icon_points[0]) ||
                !zox_tst_ui_pointer_position(world, canvas, icons[1], &icon_points[1])) {
                return 0;
            }
            zox_tst_ui_capture("taskbar.ppm");
            break;
        case 2: {
            zox_tst_waiting = "the taskbar screenshot";
            if (zox_screenshot_request[0]) {
                return 0;
            }
            int2 size = int2_zero;
            byte* pixels = zox_tst_load_ppm(zox_tst_path("taskbar.ppm"), &size);
            for (int i = 0; i < 2; i++) {
                zox_tst_check(zox_tst_ui_pixel_is(pixels, size, icon_points[i], zox_tst_taskbar_colors[i]),
                    "%s icon did not draw at [%i,%i]", zox_tst_taskbar_names[i], icon_points[i].x, icon_points[i].y);
            }
            free(pixels);
            break;
        }
        case 3:
            if (closing) {
                zox_tst_waiting = "the fixture window's close button";
                const entity window = zox_tst_taskbar_windows[index];
                if (zox_getv(window, LocalScale1) != 1) {
                    return 0;
                }
                const entity close = zox_get_child_by_id_recursive(world, window, zox_id(CloseButton));
                const byte found = zox_valid(close) && zox_tst_ui_pointer_position(world, canvas, close, &point);
                zox_tst_check(found, "%s has no positioned close button", zox_tst_taskbar_names[index]);
                if (!found) {
                    return 1;
                }
            } else {
                zox_tst_waiting = "the hovered taskbar icon's tooltip";
                point = icon_points[index];
                zox_tst_mouse(point, 0);
                const entity tip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
                if (!zox_valid(tip) || zox_get_link(world, tip, TooltipLink) != icons[index]) {
                    return 0;
                }
                char* text = get_zext_text(zox_get(tip, TextData));
                zox_tst_check(text && !strcmp(text, zox_tst_taskbar_names[index]), "%s icon showed tooltip [%s]",
                    zox_tst_taskbar_names[index], text ? text : "empty");
                free(text);
            }
            break;
        case 4: {
            zox_tst_waiting = closing ? "clicking the fixture window's close button" : "clicking the fixture taskbar icon";
            if (!zox_tst_ui_click_at(point, &click_step)) {
                return 0;
            }
            const entity window = zox_get_child_by_id(world, canvas, zox_tst_taskbar_ids[index]);
            if (closing) {
                const byte closed = window == zox_tst_taskbar_windows[index] && zox_has(window, UIClosing) &&
                    !zox_getv(frames[index], ActiveState);
                zox_tst_check(closed, "%s close did not dismiss its window and clear its frame", zox_tst_taskbar_names[index]);
                if (!closed) {
                    return 1;
                }
            } else {
                const byte opened = zox_valid(window) && zox_get_link(world, window, ToggleLink) == frames[index] &&
                    zox_getv(frames[index], ActiveState);
                zox_tst_check(opened, "%s click did not open and light its own window", zox_tst_taskbar_names[index]);
                if (!opened) {
                    return 1;
                }
                zox_tst_taskbar_windows[index] = window;
            }
            const color expected_outline = closing ? zox_getv(frames[index], ElementOutlineColor) : zox_getv(frames[index], ActiveColor);
            zox_tst_check(color_equals(zox_getv(frames[index], OutlineColor), expected_outline),
                "%s frame did not change its outline on %s", zox_tst_taskbar_names[index], closing ? "close" : "open");
            const int other = 1 - index;
            zox_tst_check(zox_get_child_by_id(world, canvas, zox_tst_taskbar_ids[other]) == zox_tst_taskbar_windows[other] &&
                !!zox_getv(frames[other], ActiveState) == !!zox_tst_taskbar_windows[other],
                "%s click changed the other entry", zox_tst_taskbar_names[index]);
            break;
        }
        default:
            zox_tst_waiting = "the closed fixture window to be deleted";
            if (closing && zox_valid(zox_tst_taskbar_windows[index])) {
                return 0;
            }
            if (closing) {
                zox_tst_taskbar_windows[index] = 0;
            }
            step = 3;
            return ++action == sizeof(entries) / sizeof(entries[0]);
    }
    step++;
    return 0;
}
