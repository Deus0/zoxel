// tst: editor, opens and uses the editor tools from the main menu (no realm, so it's quick)
//  - hierarchy (y): lists the entity tree from the root, one button per entity, at most
//      hierarchy_max_elements, each targeting a unique, valid entity
//  - clicking a hierarchy row opens the inspector on its entity
//  - inspector: a row for the name, parent, children, then one per id (component, tag, pair),
//      every row targeting the inspected entity, shown component rows show their live label
//  - inspect_entity retargets it: the old rows are replaced by the new entity's rows
//  - closing the inspector, then inspect_entity, doesn't touch the deleted window (no errors)
//  - fps display (z) and debug label toggle on and off
//  - with every tool closed, the canvas is back to the entities it started with (no leaks)
//  - run: bash bsh/tst.sh editor
#define zox_tst_editor_settle_frames 10
int zox_tst_editor_stage = 0;
int zox_tst_editor_frames = 0;
int zox_tst_editor_baseline = 0;
uint zox_tst_editor_errors = 0;
entity zox_tst_editor_target = 0;

static inline void zox_tst_editor_next() {
    zox_tst_editor_stage++;
    zox_tst_editor_frames = 0;
}

// the list of an editor window (hierarchy or inspector)
entity zox_tst_editor_list(ecs* world, entity window) {
    const entity scrollview = zox_valid(window) ? zox_get_link(world, window, ScrollviewLink) : 0;
    return zox_valid(scrollview) ? zox_get_link(world, scrollview, ListUILink) : 0;
}

// counts the list's rows, and the rows not targeting target (if target is set)
int zox_tst_editor_rows(ecs* world, entity list, entity target, int* wrong) {
    int count = 0;
    *wrong = 0;
    iter it2 = zox_children(world, list);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity row = it2.entities[j];
            if (!zox_has(row, EntityTarget)) {
                continue;
            }
            count++;
            if (target && zox_getv(row, EntityTarget) != target) {
                (*wrong)++;
            }
        }
    }
    return count;
}

byte zox_tst_editor_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_editor_frames++;
    if (zox_tst_editor_frames < zox_tst_editor_settle_frames) {
        return 0;
    }
    const entity hierarchy = zox_tst_menu(world, canvas, zox_id(HierarchyUI));
    const entity inspector = zox_tst_menu(world, canvas, zox_id(InspectorUI));
    switch (zox_tst_editor_stage) {
        case 0:
            zox_tst_waiting = "the main menu";
            if (!zox_tst_main_menu(world, player, canvas)) {
                zox_tst_editor_frames = 0;
                return 0;
            }
            break;
        case 1:
            zox_tst_editor_baseline = zox_tst_count_descendants(world, canvas);
            zox_log("[tst] canvas starts with [%i] entities, opening the hierarchy", zox_tst_editor_baseline);
            toggle_hierarchy(world, zox_key_y);
            break;
        case 2: {
            zox_tst_waiting = "the hierarchy's rows";
            const entity list = zox_tst_editor_list(world, hierarchy);
            int wrong;
            const int rows = zox_valid(list) ? zox_tst_editor_rows(world, list, 0, &wrong) : 0;
            if (!rows) {
                return 0;
            }
            zox_tst_check(rows <= hierarchy_max_elements, "hierarchy has [%i] rows, over the [%i] limit", rows, hierarchy_max_elements);
            // every row targets a unique, valid entity, the first one is the root
            const entity root = zox_getv(hierarchy, EntityTarget);
            int invalid = 0;
            int duplicates = 0;
            byte has_root = 0;
            entity seen[hierarchy_max_elements];
            int seen_count = 0;
            entity clickable = 0;
            iter it2 = zox_children(world, list);
            while (zox_children_next(it2)) {
                for (int j = 0; j < it2.count; j++) {
                    const entity row = it2.entities[j];
                    if (!zox_has(row, EntityTarget)) {
                        continue;
                    }
                    const entity target = zox_getv(row, EntityTarget);
                    if (!zox_valid(target)) {
                        invalid++;
                        continue;
                    }
                    for (int k = 0; k < seen_count; k++) {
                        if (seen[k] == target) {
                            duplicates++;
                            break;
                        }
                    }
                    if (seen_count < hierarchy_max_elements) {
                        seen[seen_count++] = target;
                    }
                    if (target == root) {
                        has_root = 1;
                    } else if (!clickable && !zox_has(target, EditorElement)) {
                        clickable = row;
                    }
                }
            }
            zox_log("[tst] hierarchy of [%s] has [%i] rows", zox_getn(root), rows);
            zox_tst_check(invalid == 0, "hierarchy has [%i] rows targeting invalid entities", invalid);
            zox_tst_check(duplicates == 0, "hierarchy has [%i] duplicate rows", duplicates);
            zox_tst_check(has_root, "hierarchy has no row for its root [%s]", zox_getn(root));
            zox_tst_check(clickable, "hierarchy has no row to click");
            if (!clickable) {
                return 1;
            }
            zox_tst_editor_target = zox_getv(clickable, EntityTarget);
            zox_log("[tst] clicking hierarchy row [%s]", zox_getn(zox_tst_editor_target));
            zox_tst_click(world, player, clickable);
            break;
        }
        case 3:
        case 5: {
            // inspector rows: name, parent, children, then one per id, all targeting the entity
            zox_tst_waiting = zox_tst_editor_stage == 3 ? "the inspector, from the hierarchy" : "the inspector, retargeted";
            const entity list = zox_tst_editor_list(world, inspector);
            if (!zox_valid(list) || zox_getv(inspector, EntityTarget) != zox_tst_editor_target) {
                return 0;
            }
            int wrong;
            const int rows = zox_tst_editor_rows(world, list, zox_tst_editor_target, &wrong);
            const int expected = 3 + zox_get_ids_count(world, zox_tst_editor_target);
            if (rows != expected && zox_tst_editor_frames < 3 * zox_tst_editor_settle_frames) {
                return 0;
            }
            zox_log("[tst] inspector of [%s] has [%i] rows", zox_getn(zox_tst_editor_target), rows);
            zox_tst_check(rows == expected, "inspector of [%s] has [%i] rows, expected [%i]", zox_getn(zox_tst_editor_target), rows, expected);
            zox_tst_check(wrong == 0, "inspector of [%s] has [%i] rows targeting another entity", zox_getn(zox_tst_editor_target), wrong);
            if (zox_tst_editor_stage == 3) {
                zox_tst_editor_target = canvas;
                inspect_entity(world, canvas);
            } else {
                // shown component rows show their component's live label (inspector_label_system)
                int checked = 0;
                int stale = 0;
                iter it2 = zox_children(world, list);
                while (zox_children_next(it2)) {
                    for (int j = 0; j < it2.count; j++) {
                        const entity row = it2.entities[j];
                        if (!zox_has(row, ComponentTarget) || zox_getv(row, RenderDisabled)) {
                            continue;
                        }
                        const entity component = zox_getv(row, ComponentTarget);
                        const entity text = zox_get_child_by_id(world, row, zox_id(TextData));
                        if (!component || !zox_valid(text)) {
                            continue;
                        }
                        char* label = fetch_component_label(world, zox_tst_editor_target, component);
                        if (!is_zext(zox_get(text, TextData), label)) {
                            stale++;
                        }
                        free(label);
                        checked++;
                    }
                }
                zox_tst_check(checked > 0, "inspector shows no component rows");
                zox_tst_check(stale == 0, "[%i] of [%i] shown component rows don't show their component's label", stale, checked);
            }
            break;
        }
        case 4:
            // waits a stage, so the retarget above goes through
            break;
        case 6:
            zox_log("[tst] closing the inspector");
            toggle_inspector(world, zox_key_u);
            break;
        case 7:
            zox_tst_waiting = "the inspector to close";
            if (inspector) {
                return 0;
            }
            // inspecting with the inspector closed must not touch the deleted window
            zox_tst_editor_errors = zox_log_errors_count;
            inspect_entity(world, canvas);
            break;
        case 8:
            zox_tst_check(zox_log_errors_count == zox_tst_editor_errors,
                "inspect_entity with the inspector closed logged [%u] errors", zox_log_errors_count - zox_tst_editor_errors);
            toggle_fps_viewer(world, zox_key_z);
            canvas_toggle_ui(world, canvas, 0, spawn_game_debug_label);
            break;
        case 9:
            zox_tst_waiting = "the fps display and debug label";
            zox_tst_check(zox_tst_menu(world, canvas, zox_id(FPSDisplay)), "fps display didn't open");
            zox_tst_check(zox_tst_menu(world, canvas, zox_id(GameDebugLabel)), "debug label didn't open");
            toggle_fps_viewer(world, zox_key_z);
            canvas_toggle_ui(world, canvas, zox_tst_menu(world, canvas, zox_id(GameDebugLabel)), spawn_game_debug_label);
            toggle_hierarchy(world, zox_key_y);
            break;
        default: {
            zox_tst_waiting = "every editor tool to close";
            if (hierarchy ||
                inspector ||
                zox_tst_menu(world, canvas, zox_id(FPSDisplay)) ||
                zox_tst_menu(world, canvas, zox_id(GameDebugLabel)))
            {
                return 0;
            }
            const int entities = zox_tst_count_descendants(world, canvas);
            zox_tst_check(entities == zox_tst_editor_baseline,
                "with the editor closed the canvas has [%i] entities, it started with [%i]", entities, zox_tst_editor_baseline);
            return 1;
        }
    }
    zox_tst_editor_next();
    return 0;
}

void zox_tst_editor_check(ecs* world) {
    (void) world;
}

void zox_tsts_editor() {
    zox_tst_add("editor", zox_tst_editor_ready, zox_tst_editor_check, 60);
    zox_tst_set_setup("editor", NULL, 1);
}
