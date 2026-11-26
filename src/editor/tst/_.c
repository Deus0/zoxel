// tst: editor, opens and uses the editor tools from the main menu (no realm, so it's quick)
//  - hierarchy (y): lists scene roots in slots, each targeting a unique, valid entity
//  - clicking a hierarchy row opens the inspector on its entity
//  - inspector: a row for the name, parent, children, then one per id (component, tag, pair),
//      every row targeting the inspected entity, shown component rows show their live label
//  - inspect_entity retargets it: the old rows are replaced by the new entity's rows
//  - closing the inspector, then inspect_entity, doesn't touch the deleted window (no errors)
//  - fps display (z) and debug label toggle on and off
//  - with every tool closed, only the persistent menubar remains from the editor
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
            if (!zox_has(row, Target)) {
                continue;
            }
            count++;
            if (target && zox_getv(row, Target) != target) {
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
            if (!list) {
                return 0;
            }
            entity slots[UINT8_MAX + 1];
            const int count = get_list_children_ordered(world, list, slots, UINT8_MAX + 1);
            if (!count || !zox_get_link(world, slots[0], RowTargetLink)) {
                return 0;
            }
            zox_tst_check(count == zox_getv(list, ListVisible) + 1, "wrong hierarchy slot count");
            for (int i = 0; i < count; i++) {
                const entity target = zox_get_link(world, slots[i], RowTargetLink);
                for (int j = 0; target && j < i; j++) {
                    zox_tst_check(target != zox_get_link(world, slots[j], RowTargetLink), "duplicate hierarchy target");
                }
            }
            const entity clickable = slots[0];
            zox_tst_check(zox_getv(clickable, ChildIndex) == 0 && !zox_getv(clickable, RenderDisabled),
                "first hierarchy slot is unavailable");
            zox_tst_editor_target = zox_get_link(world, clickable, RowTargetLink);
            zox_log("[tst] clicking hierarchy row [%s]", zox_getn(zox_tst_editor_target));
            zox_tst_click(world, player, clickable);
            break;
        }
        case 3:
        case 5: {
            // inspector rows: name, parent, children, then one per id, all targeting the entity
            zox_tst_waiting = zox_tst_editor_stage == 3 ? "the inspector, from the hierarchy" : "the inspector, retargeted";
            const entity list = zox_tst_editor_list(world, inspector);
            if (!zox_valid(list) || zox_getv(inspector, Target) != zox_tst_editor_target) {
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
                        if (!is_zext((TextData*) zox_get(text, TextData), label)) {
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
            const entity bar = zox_get_child_by_id(world, canvas, zox_id(Menubar));
            const int entities = zox_tst_count_descendants(world, canvas) - 1 - zox_tst_count_descendants(world, bar);
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

#include "hierarchy_fold.c"
#include "file_window.c"
#include "search.c"

#include "hierarchy_big.c"
#include "menubar.c"
#include "realm.c"

void zox_tsts_editor() {
    zox_tst_add("editor_menubar", zox_tst_editor_menubar_ready, zox_tst_ui_fixture_check, 20);
    zox_tst_set_setup("editor_menubar", NULL, 1);
    zox_tst_add("editor_realm", zox_tst_editor_realm_ready, zox_tst_ui_fixture_check, 60);
    zox_tst_set_setup("editor_realm", NULL, 1);
    zox_tst_add("hierarchy_big", zox_tst_hierarchy_big_ready, zox_tst_hierarchy_search_check, 20);
    zox_tst_set_setup("hierarchy_big", NULL, 1);
    zox_tst_add("editor", zox_tst_editor_ready, zox_tst_editor_check, 60);
    zox_tst_set_setup("editor", NULL, 1);
    zox_tst_add("hierarchy_fold", zox_tst_hierarchy_fold_ready, zox_tst_hierarchy_fold_check, 20);
    zox_tst_set_setup("hierarchy_fold", NULL, 1);
    zox_tst_add("hierarchy_roots", zox_tst_hierarchy_roots_ready, zox_tst_hierarchy_search_check, 20);
    zox_tst_set_setup("hierarchy_roots", NULL, 1);
    zox_tst_add("file_window", zox_tst_file_window_ready, zox_tst_file_window_check, 20);
    zox_tst_set_setup("file_window", NULL, 1);
    zox_tst_add("hierarchy_search", zox_tst_hierarchy_search_ready, zox_tst_hierarchy_search_check, 20);
    zox_tst_set_setup("hierarchy_search", NULL, 1);
    zox_tst_add("files_search", zox_tst_files_search_ready, zox_tst_files_search_check, 20);
    zox_tst_set_setup("files_search", NULL, 1);
    zox_tst_add("inspector_search", zox_tst_inspector_search_ready, zox_tst_hierarchy_search_check, 20);
    zox_tst_set_setup("inspector_search", NULL, 1);
}
