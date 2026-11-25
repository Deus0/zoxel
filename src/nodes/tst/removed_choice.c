// tst: node_removed_choice, deleting a pending choice leaves the runner free to use its default edge.
static entity zox_tst_removed_choice_process;
static entity zox_tst_removed_choice_fallback;

static byte zox_tst_node_removed_choice_ready(ecs* world) {
    static entity branch, choice;
    static int step;
    const entity process = zox_tst_removed_choice_process;
    zox_tst_progress(step);
    switch (step) {
        case 0:
            if (!zox_tst_node_prefabs_ready(world)) {
                return 0;
            }
            branch = spawn_node_leaf(world, prefab_node);
            choice = spawn_node_leaf(world, prefab_node);
            zox_tst_removed_choice_fallback = spawn_node_leaf(world, prefab_node);
            zox_link(world, branch, NodeLink, choice);
            zox_link(world, branch, NodeLink, zox_tst_removed_choice_fallback);
            zox_tst_removed_choice_process = zox_tst_nodes_spawn_process(world, branch);
            break;
        case 1:
            zox_tst_waiting = "the process at the branch before removing its choice";
            if (zox_tst_nodes_current(world, process) != branch) {
                return 0;
            }
            zox_link(world, process, NextNodeLink, choice);
            zox_tst_check(zox_get_link(world, process, NextNodeLink) == choice, "the process did not record its pending choice");
            zox_delete(choice);
            zox_tst_check(!zox_get_link(world, process, NextNodeLink), "deleting the choice left a stale pending link");
            zox_add(process, TriggerEnd);
            break;
        case 2:
            zox_tst_waiting = "the default edge to replace a removed choice";
            if (zox_tst_nodes_current(world, process) == branch) {
                return 0;
            }
            zox_tst_check(zox_tst_nodes_current(world, process) == zox_tst_removed_choice_fallback,
                "a removed choice prevented the process from taking its default edge");
            if (!zox_valid(process)) {
                return 1;
            }
            zox_add(process, TriggerEnd);
            break;
        default:
            zox_tst_waiting = "the process to exit after using its default edge";
            return !zox_valid(process);
    }
    step++;
    return 0;
}

static void zox_tst_node_removed_choice_check(ecs* world) {
    zox_tst_check(zox_valid(zox_tst_removed_choice_fallback), "finishing the process deleted its fallback node");
}
