// tst: hierarchy_fold, root and parent have arrows, the parent opens and closes its two leaf rows
//  - run: bash bsh/tst.sh hierarchy_fold
int zox_tst_hierarchy_fold_stage = 0;
entity zox_tst_hierarchy_fold_window = 0;
byte zox_tst_hierarchy_root_opened = 0;
entity zox_tst_hierarchy_fold_root = 0;
entity zox_tst_hierarchy_fold_parent = 0;

// rows in the window's list, 0 while it's waiting on its refresh
int zox_tst_hierarchy_fold_rows(ecs* world) {
    const entity list = zox_tst_editor_list(world, zox_tst_hierarchy_fold_window);
    int wrong;
    return zox_valid(list) && !zox_has(list, Dirty) ?
        zox_tst_editor_rows(world, list, 0, &wrong) : 0;
}

// Check every row and return the parent's arrow.
static entity zox_tst_hierarchy_fold_arrows(ecs* world) {
    const entity list = zox_tst_editor_list(world, zox_tst_hierarchy_fold_window);
    entity parent_arrow = 0;
    iter children = zox_children(world, list);
    while (zox_children_next(children)) {
        for (int i = 0; i < children.count; i++) {
            const entity row = children.entities[i];
            const entity target = zox_getv(row, Target);
            const entity arrow = zox_get_child_by_id(world, row, zox_id(Button));
            const byte has_children = target == zox_tst_hierarchy_fold_root || target == zox_tst_hierarchy_fold_parent;
            zox_tst_check(!!arrow == has_children, "wrong arrow presence on [%s]", zox_getn(target));
            zox_tst_check(zox_getv(row, ClickEvent) == button_event_clicked_hierarchy,
                "row [%s] lost its inspect callback", zox_getn(target));
            if (arrow) {
                const entity image = zox_get_child_by_id(world, arrow, zox_id(Element));
                zox_tst_check(image, "row [%s] has no arrow image", zox_getn(target));
            }
            if (target == zox_tst_hierarchy_fold_parent) {
                parent_arrow = arrow;
            }
        }
    }
    return parent_arrow;
}

byte zox_tst_hierarchy_fold_ready(ecs* world) {
    const entity canvas = get_editor_canvas(world);
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the editor canvas";
        return 0;
    }
    const int rows = zox_tst_hierarchy_fold_window ? zox_tst_hierarchy_fold_rows(world) : 0;
    switch (zox_tst_hierarchy_fold_stage) {
        case 0: {
            // root > parent > two children
            const entity root = zox_new();
            zox_set_name(root, "fold_root");
            const entity parent = zox_new();
            zox_set_name(parent, "fold_parent");
            zox_set_parent(world, parent, root);
            for (int i = 0; i < 2; i++) {
                const entity child = zox_new();
                zox_set_parent(world, child, parent);
            }
            zox_tst_hierarchy_fold_root = root;
            zox_tst_hierarchy_fold_parent = parent;
            zox_defer_begin();
            zox_tst_hierarchy_fold_window = spawn_window_hierarchy(world, canvas, 0, root);
            zox_defer_end();
            break;
        }
        case 1: {
            zox_tst_waiting = "the root and its folded parent";
            if (rows == 1 && !zox_tst_hierarchy_root_opened) {
                // the root starts folded, one row: unfold it
                zox_tst_hierarchy_root_opened = 1;
                unfold_toggle(world, zox_tst_editor_list(world, zox_tst_hierarchy_fold_window), zox_tst_hierarchy_fold_root);
                return 0;
            }
            if (rows != 2) {
                return 0;
            }
            const entity list = zox_tst_editor_list(world, zox_tst_hierarchy_fold_window);
            zox_setv(list, ListVisible, 1);
            set_list_start(world, list, 1);
            const entity arrow = zox_tst_hierarchy_fold_arrows(world);
            if (arrow) {
                zox_getv(arrow, ClickEvent)(world, (ClickEventData) { .clicked = arrow });
            }
            break;
        }
        case 2: {
            zox_tst_waiting = "the parent unfolded, four rows";
            if (rows != 4) {
                return 0;
            }
            const entity list = zox_tst_editor_list(world, zox_tst_hierarchy_fold_window);
            zox_tst_check(zox_getv(list, ListStart) == 1, "unfold reset the scroll position");
            const entity arrow = zox_tst_hierarchy_fold_arrows(world);
            if (arrow) {
                zox_getv(arrow, ClickEvent)(world, (ClickEventData) { .clicked = arrow });
            }
            break;
        }
        case 3: {
            zox_tst_waiting = "the parent folded again, two rows";
            if (rows != 2) {
                return 0;
            }
            const entity list = zox_tst_editor_list(world, zox_tst_hierarchy_fold_window);
            zox_tst_check(zox_getv(list, ListStart) == 1, "fold reset the scroll position");
            zox_tst_hierarchy_fold_arrows(world);
            return 1;
        }
    }
    zox_tst_hierarchy_fold_stage++;
    return 0;
}

void zox_tst_hierarchy_fold_check(ecs* world) {
    const entity list = zox_tst_editor_list(world, zox_tst_hierarchy_fold_window);
    const Unfolded* unfolded = zox_get(list, Unfolded);
    zox_tst_check(unfolded->length == 1, "[%i] entities left unfolded after folding, not the root only", unfolded->length);
    zox_tst_check(zox_tst_hierarchy_root_opened, "the root never started folded");
    zox_tst_check(zox_log_errors_count == 0, "[%u] errors logged", zox_log_errors_count);
}

// tst: hierarchy_roots, a hierarchy with no root target lists every root entity, and nothing of the editor's ui
byte zox_tst_hierarchy_roots_ready(ecs* world) {
    static int step;
    static entity window, root_a, root_b;
    const entity canvas = get_editor_canvas(world);
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the editor canvas";
        return 0;
    }
    const entity list = window ? zox_tst_editor_list(world, window) : 0;
    if (step == 0) {
        root_a = zox_new();
        zox_set_name(root_a, "scene_root_a");
        root_b = zox_new();
        zox_set_name(root_b, "scene_root_b");
        zox_defer_begin();
        window = spawn_window_hierarchy(world, canvas, 0, 0);
        zox_defer_end();
        step = 1;
        return 0;
    }
    zox_tst_waiting = "both roots as rows";
    if (!list || zox_has(list, Dirty)) {
        return 0;
    }
    byte found_a = 0, found_b = 0;
    int editor_rows = 0, rows = 0;
    iter it2 = zox_children(world, list);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            if (!zox_has(it2.entities[j], Target)) {
                continue;
            }
            const entity target = zox_getv(it2.entities[j], Target);
            rows++;
            found_a |= target == root_a;
            found_b |= target == root_b;
            editor_rows += zox_has(target, EditorElement) || zox_get_parent_by_id(world, target, zox_id(EditorElement));
        }
    }
    if (!found_a || !found_b) {
        return 0;
    }
    zox_log("[tst] hierarchy_roots: [%i] root rows", rows);
    zox_tst_check(editor_rows == 0, "[%i] rows of the editor's own ui", editor_rows);
    return 1;
}
