// tst: hierarchy_fold, root and parent have arrows, the parent opens and closes its two leaf rows
//  - run: bash bsh/tst.sh hierarchy_fold
int zox_tst_hierarchy_fold_stage = 0;
entity zox_tst_hierarchy_fold_window = 0;
byte zox_tst_hierarchy_root_opened = 0;
entity zox_tst_hierarchy_fold_root = 0;
entity zox_tst_hierarchy_fold_parent = 0;

static int zox_tst_hierarchy_count(ecs* world, entity window) {
    const entity list = zox_tst_editor_list(world, window);
    if (!list || zox_has(list, Dirty)) {
        return 0;
    }
    // the slots fill in observers after the walk, and respawned ones show a frame before they're bound: wait until
    //  every shown slot has its entity
    iter slots = zox_children(world, list);
    while (zox_children_next(slots)) {
        for (int i = 0; i < slots.count; i++) {
            const entity slot = slots.entities[i];
            if (zox_has(slot, Dirty) || (!zox_getv(slot, RenderDisabled) && !zox_get_link(world, slot, RowTargetLink))) {
                zox_children_end(slots);
                return 0;
            }
        }
    }
    HierarchyWalk walk = hierarchy_walk_begin(world, window, list);
    hierarchy_walk(world, &walk);
    return walk.index;
}

// Check every row and return the parent's arrow.
static entity zox_tst_hierarchy_fold_arrows(ecs* world) {
    const entity list = zox_tst_editor_list(world, zox_tst_hierarchy_fold_window);
    entity parent_arrow = 0;
    iter children = zox_children(world, list);
    while (zox_children_next(children)) {
        for (int i = 0; i < children.count; i++) {
            const entity row = children.entities[i];
            const entity target = zox_get_link(world, row, RowTargetLink);
            const entity arrow = zox_get_child_by_id(world, row, zox_id(FoldArrow));
            const byte has_children = target == zox_tst_hierarchy_fold_root || target == zox_tst_hierarchy_fold_parent;
            zox_tst_check(arrow && zox_getv(arrow, RenderDisabled) == !has_children,
                "wrong arrow visibility on [%s] slot [%u] slot hidden [%i] arrow hidden [%i] stage [%i]", zox_getn(target),
                zox_getv(row, ChildIndex), zox_getv(row, RenderDisabled), arrow ? zox_getv(arrow, RenderDisabled) : -1,
                zox_tst_hierarchy_fold_stage);
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
    const int rows = zox_tst_hierarchy_fold_window ? zox_tst_hierarchy_count(world, zox_tst_hierarchy_fold_window) : 0;
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
            scroll_virtual_list_rows(world, list, 1);
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
            zox_tst_check(zox_get_link(world, list, ListStartLink) == zox_tst_hierarchy_fold_parent, "unfold reset the scroll position");
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
            zox_tst_check(zox_get_link(world, list, ListStartLink) == zox_tst_hierarchy_fold_parent, "fold reset the scroll position");
            zox_tst_hierarchy_fold_arrows(world);
            return 1;
        }
    }
    zox_tst_hierarchy_fold_stage++;
    return 0;
}

void zox_tst_hierarchy_fold_check(ecs* world) {
    const entity list = zox_tst_editor_list(world, zox_tst_hierarchy_fold_window);
    zox_tst_check(zox_get_links_count(world, list, Unfolded) == 1, "expected only the root to remain unfolded");
    zox_tst_check(zox_tst_hierarchy_root_opened, "the root never started folded");
    zox_tst_check(zox_log_errors_count == 0, "[%u] errors logged", zox_log_errors_count);
}

// tst: hierarchy_roots, a hierarchy with no root target lists every root entity, and nothing of the editor's ui
byte zox_tst_hierarchy_roots_ready(ecs* world) {
    static int step;
    static entity window, root_a, root_b;
    static byte found_a, found_b;
    static int scanned;
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
    int editor_rows = 0, rows = 0;
    iter it2 = zox_children(world, list);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity target = zox_get_link(world, it2.entities[j], RowTargetLink);
            if (!target) {
                continue;
            }
            rows++;
            found_a |= target == root_a;
            found_b |= target == root_b;
            editor_rows += zox_has(target, EditorElement) || zox_get_parent_by_id(world, target, zox_id(EditorElement));
        }
    }
    const int count = zox_tst_hierarchy_count(world, window);
    zox_tst_check(editor_rows == 0, "[%i] rows of the editor's own ui", editor_rows);
    if (scanned + rows < count) {
        const int advance = zox_getv(list, ListVisible);
        scanned += advance;
        scroll_virtual_list_rows(world, list, advance);
        return 0;
    }
    zox_log("[tst] hierarchy_roots: [%i] root rows", count);
    zox_tst_check(found_a && found_b, "expected both fixture roots in the walk");
    return 1;
}
