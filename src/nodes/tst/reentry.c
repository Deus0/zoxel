// tst: node_reentry, immediate Begin observers can walk a loop, revisit its entry, and choose an exit.
static entity zox_tst_reentry_nodes[3];
static entity zox_tst_reentry_process;
static entity zox_tst_reentry_history[4];
static int zox_tst_reentry_count;

static void zox_tst_node_reentry_begin(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        const entity process = it->entities[i];
        if (process != zox_tst_reentry_process) {
            continue;
        }
        const entity current = zox_tst_nodes_current(world, process);
        const int visit = zox_tst_reentry_count++;
        if (visit >= 4) {
            continue;
        }
        zox_tst_reentry_history[visit] = current;
        if (current == zox_tst_reentry_nodes[0]) {
            zox_link(world, process, NextNodeLink, zox_tst_reentry_nodes[visit == 0 ? 1 : 2]);
        }
        delayed_node_end(world, process);
    }
}

static byte zox_tst_node_reentry_ready(ecs* world) {
    if (!zox_tst_reentry_process) {
        if (!zox_tst_node_prefabs_ready(world)) {
            return 0;
        }
        for (int i = 0; i < 3; i++) {
            zox_tst_reentry_nodes[i] = spawn_node_leaf(world, prefab_node);
        }
        const entity* nodes = zox_tst_reentry_nodes;
        zox_link(world, nodes[0], NodeLink, nodes[1]);
        zox_link(world, nodes[0], NodeLink, nodes[2]);
        zox_link(world, nodes[1], NodeLink, nodes[0]);
        zox_tst_reentry_process = zox_tst_nodes_spawn_process(world, nodes[0]);
        return 0;
    }
    zox_tst_waiting = "the Begin observer to walk the loop and reach its exit";
    zox_tst_progress(zox_tst_reentry_count);
    return !zox_valid(zox_tst_reentry_process) || zox_tst_reentry_count > 4;
}

static void zox_tst_node_reentry_check(ecs* world) {
    const int expected[] = { 0, 1, 0, 2 };
    zox_tst_check(!zox_valid(zox_tst_reentry_process), "the loop did not exit");
    zox_tst_check(zox_tst_reentry_count == 4, "the loop emitted [%i] Begin events, expected four", zox_tst_reentry_count);
    for (int i = 0; i < 4; i++) {
        zox_tst_check(zox_tst_reentry_history[i] == zox_tst_reentry_nodes[expected[i]], "Begin visit [%i] had the wrong current node", i);
    }
}
