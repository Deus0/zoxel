// tst: node_parallel, two runners can choose independently on the same graph and finish separately.
static entity zox_tst_parallel_nodes[4];
static entity zox_tst_parallel_processes[2];

static byte zox_tst_node_parallel_ready(ecs* world) {
    static int step;
    entity* nodes = zox_tst_parallel_nodes;
    entity* process = zox_tst_parallel_processes;
    zox_tst_progress(step);
    switch (step) {
        case 0:
            if (!zox_tst_node_prefabs_ready(world)) {
                return 0;
            }
            for (int i = 0; i < 4; i++) {
                nodes[i] = spawn_node_leaf(world, prefab_node);
            }
            zox_link(world, nodes[0], NodeLink, nodes[1]);
            zox_link(world, nodes[0], NodeLink, nodes[2]);
            zox_link(world, nodes[1], NodeLink, nodes[3]);
            zox_link(world, nodes[2], NodeLink, nodes[3]);
            process[0] = zox_tst_nodes_spawn_process(world, nodes[0]);
            process[1] = zox_tst_nodes_spawn_process(world, nodes[0]);
            break;
        case 1:
            zox_tst_waiting = "both processes at the shared branch";
            if (zox_tst_nodes_current(world, process[0]) != nodes[0] || zox_tst_nodes_current(world, process[1]) != nodes[0]) {
                return 0;
            }
            // Replacing one process's pending selection must leave the other process's selection intact.
            zox_link(world, process[0], NextNodeLink, nodes[2]);
            zox_link(world, process[1], NextNodeLink, nodes[2]);
            zox_link(world, process[0], NextNodeLink, nodes[1]);
            zox_tst_check(!zox_get_link_index(world, process[0], NextNodeLink, 1), "replacing a pending choice left two choices");
            zox_add(process[0], TriggerEnd);
            zox_add(process[1], TriggerEnd);
            break;
        case 2:
            zox_tst_waiting = "both processes to take their independent choices";
            if (zox_tst_nodes_current(world, process[0]) == nodes[0] || zox_tst_nodes_current(world, process[1]) == nodes[0]) {
                return 0;
            }
            zox_tst_check(zox_tst_nodes_current(world, process[0]) == nodes[1] && zox_tst_nodes_current(world, process[1]) == nodes[2],
                "the two processes did not take their independent branches");
            zox_tst_check(!zox_get_link(world, process[0], NextNodeLink) && !zox_get_link(world, process[1], NextNodeLink),
                "one of the processes retained its used choice");
            zox_add(process[0], TriggerEnd);
            break;
        case 3:
            zox_tst_waiting = "the first process at the join";
            if (zox_tst_nodes_current(world, process[0]) != nodes[3]) {
                return 0;
            }
            zox_tst_check(zox_tst_nodes_current(world, process[1]) == nodes[2], "advancing the first process moved the second one");
            zox_add(process[0], TriggerEnd);
            break;
        case 4:
            zox_tst_waiting = "the first process to exit while the second stays active";
            if (zox_valid(process[0])) {
                return 0;
            }
            zox_tst_check(zox_tst_nodes_current(world, process[1]) == nodes[2], "finishing the first process affected the second one");
            zox_add(process[1], TriggerEnd);
            break;
        case 5:
            zox_tst_waiting = "the second process to reach the same join after the first exits";
            if (zox_tst_nodes_current(world, process[1]) != nodes[3]) {
                return 0;
            }
            zox_add(process[1], TriggerEnd);
            break;
        default:
            zox_tst_waiting = "the second process to exit";
            return !zox_valid(process[1]);
    }
    step++;
    return 0;
}

static void zox_tst_node_parallel_check(ecs* world) {
    for (int i = 0; i < 4; i++) {
        zox_tst_check(zox_valid(zox_tst_parallel_nodes[i]), "finishing a process deleted shared node [%i]", i);
    }
}
