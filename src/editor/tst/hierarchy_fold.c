// tst: hierarchy_fold, a parent with two children in a hierarchy: one row, unfolded three, folded one again
//  - only the parent's row has children (HierarchyRow.children), the root starts unfolded, the rest folded
//  - run: bash bsh/tst.sh hierarchy_fold
int zox_tst_hierarchy_fold_stage = 0;
entity zox_tst_hierarchy_fold_window = 0;
entity zox_tst_hierarchy_fold_parent = 0;

// rows in the window's list, 0 while it's waiting on its refresh
int zox_tst_hierarchy_fold_rows(ecs* world) {
    const entity list = zox_tst_editor_list(world, zox_tst_hierarchy_fold_window);
    int wrong;
    return zox_valid(list) && !zox_has(zox_tst_hierarchy_fold_window, HierarchyUIDirty) ?
        zox_tst_editor_rows(world, list, 0, &wrong) : 0;
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
            zox_tst_hierarchy_fold_parent = parent;
            zox_tst_hierarchy_fold_window = spawn_window_hierarchy(world, canvas, 0, root);
            break;
        }
        case 1:
            zox_tst_waiting = "the root and its folded parent";
            if (rows != 2) {
                return 0;
            }
            hierarchy_fold(world, zox_tst_hierarchy_fold_window, zox_tst_hierarchy_fold_parent);
            break;
        case 2:
            zox_tst_waiting = "the parent unfolded, four rows";
            if (rows != 4) {
                return 0;
            }
            hierarchy_fold(world, zox_tst_hierarchy_fold_window, zox_tst_hierarchy_fold_parent);
            break;
        case 3:
            zox_tst_waiting = "the parent folded again, two rows";
            if (rows != 2) {
                return 0;
            }
            return 1;
    }
    zox_tst_hierarchy_fold_stage++;
    return 0;
}

void zox_tst_hierarchy_fold_check(ecs* world) {
    const HierarchyUnfolded* unfolded = zox_get(zox_tst_hierarchy_fold_window, HierarchyUnfolded);
    zox_tst_check(unfolded->length == 0, "[%i] entities left unfolded after folding", unfolded->length);
    zox_tst_check(zox_log_errors_count == 0, "[%u] errors logged", zox_log_errors_count);
}
