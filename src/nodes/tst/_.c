// tst: node_graphs, a process walks a node graph: start, advance, branch, end
//  - a graph: a -> b, b branches to c (NodeFalse) and d (NodeTrue), d -> e
//  - a process starts at its first node (a), ending a node moves it along its link (b)
//  - a branch keeps all its links (NodeLink was exclusive, branches kept only their last)
//  - a NextNodeLink picks the branch (d), and is used up, then e, then the end: the process exits
//  - Begin is added once per node, with the node current (quest nodes observe it, see quests/eve)
//  - one current node at a time (CurrentNodeLink is exclusive)
//  - a second process with no choice at the branch still moves on (to its first link)
//  - run: bash bsh/tst.sh node_graphs
#define zox_tst_nodes_max 8
int zox_tst_nodes_stage = 0;
int zox_tst_nodes_frames = 0;
entity zox_tst_nodes[5];
entity zox_tst_nodes_process = 0;
entity zox_tst_nodes_process2 = 0;
entity zox_tst_nodes_observer = 0;
entity zox_tst_nodes_begun[zox_tst_nodes_max];
int zox_tst_nodes_begun_count = 0;

static void zox_tst_nodes_on_begin(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        if (it->entities[i] == zox_tst_nodes_process && zox_tst_nodes_begun_count < zox_tst_nodes_max) {
            zox_tst_nodes_begun[zox_tst_nodes_begun_count++] = zox_get_link(world, it->entities[i], CurrentNodeLink);
        }
    }
}

static inline entity zox_tst_nodes_spawn_process(ecs* world, entity first) {
    zox_instance(prefab_process);
    zox_name("tst_process");
    zox_link(world, e, NodeLink, first);
    return e;
}

static inline entity zox_tst_nodes_current(ecs* world, entity process) {
    return zox_valid(process) ? zox_get_link(world, process, CurrentNodeLink) : 0;
}

// a stage gives up after 60 frames
#define zox_tst_nodes_timeout(message) {\
    if (zox_tst_nodes_frames > 60) {\
        zox_tst_check(0, message);\
        return 1;\
    }\
    return 0;\
}

// waits a few frames for the node systems (deferred), then checks the process is at node
static inline byte zox_tst_nodes_at(ecs* world, entity process, entity node, const char* waiting) {
    zox_tst_waiting = waiting;
    return zox_tst_nodes_frames >= 4 && zox_tst_nodes_current(world, process) == node;
}

byte zox_tst_node_graphs_ready(ecs* world) {
    zox_tst_nodes_frames++;
    entity* n = zox_tst_nodes;
    switch (zox_tst_nodes_stage) {
        case 0: {
            for (int i = 0; i < 5; i++) {
                n[i] = spawn_node_leaf(world, prefab_node);
            }
            zox_add(n[2], NodeFalse);
            zox_add(n[3], NodeTrue);
            zox_link(world, n[0], NodeLink, n[1]);
            zox_link(world, n[1], NodeLink, n[2]);
            zox_link(world, n[1], NodeLink, n[3]);
            zox_link(world, n[3], NodeLink, n[4]);
            ecs_observer_desc_t desc = {
                .query = { .expr = "[none] nodes.NodeRun, [none] core.Begin" },
                .callback = zox_tst_nodes_on_begin,
                .events = { EcsOnAdd },
            };
            zox_tst_nodes_observer = ecs_observer_init(world, &desc);
            zox_tst_nodes_process = zox_tst_nodes_spawn_process(world, n[0]);
            break;
        }
        case 1:
            if (!zox_tst_nodes_at(world, zox_tst_nodes_process, n[0], "the process to start at a")) {
                zox_tst_nodes_timeout("the process didn't start at its first node");
            }
            zox_tst_check(zox_get_link_index(world, n[1], NodeLink, 0) && zox_get_link_index(world, n[1], NodeLink, 1),
                "the branch kept one link, not two");
            zox_add(zox_tst_nodes_process, TriggerEnd);
            break;
        case 2:
            if (!zox_tst_nodes_at(world, zox_tst_nodes_process, n[1], "a to end, on to b")) {
                zox_tst_nodes_timeout("ending a didn't move on to b");
            }
            // the branch: choose d
            zox_link(world, zox_tst_nodes_process, NextNodeLink, n[3]);
            zox_add(zox_tst_nodes_process, TriggerEnd);
            break;
        case 3:
            if (!zox_tst_nodes_at(world, zox_tst_nodes_process, n[3], "the branch to d")) {
                zox_tst_nodes_timeout("the next node link didn't choose d");
            }
            zox_tst_check(!zox_get_link(world, zox_tst_nodes_process, NextNodeLink), "the next node link wasn't used up");
            zox_tst_check(!zox_get_link_index(world, zox_tst_nodes_process, CurrentNodeLink, 1), "the process has two current nodes");
            zox_add(zox_tst_nodes_process, TriggerEnd);
            break;
        case 4:
            if (!zox_tst_nodes_at(world, zox_tst_nodes_process, n[4], "d to e")) {
                zox_tst_nodes_timeout("ending d didn't move on to e");
            }
            zox_add(zox_tst_nodes_process, TriggerEnd);
            break;
        case 5:
            zox_tst_waiting = "the end of the graph, the process exits";
            if (zox_valid(zox_tst_nodes_process)) {
                zox_tst_nodes_timeout("the process didn't exit at the end");
            }
            // a second process, no choice at the branch
            zox_tst_nodes_process2 = zox_tst_nodes_spawn_process(world, n[1]);
            break;
        case 6:
            if (!zox_tst_nodes_at(world, zox_tst_nodes_process2, n[1], "the second process to start at b")) {
                zox_tst_nodes_timeout("the second process didn't start");
            }
            zox_add(zox_tst_nodes_process2, TriggerEnd);
            break;
        default: {
            zox_tst_waiting = "the second process past the branch";
            const entity current = zox_tst_nodes_current(world, zox_tst_nodes_process2);
            if (zox_tst_nodes_frames < 4 || current == n[1]) {
                zox_tst_nodes_timeout("with no choice, the branch didn't move on");
            }
            zox_tst_check(current == n[2] || current == n[3], "with no choice, the branch went to [%s]", current ? zox_getn(current) : "nothing");
            return 1;
        }
    }
    zox_tst_nodes_stage++;
    zox_tst_nodes_frames = 0;
    return 0;
}

void zox_tst_node_graphs_check(ecs* world) {
    const entity* n = zox_tst_nodes;
    const entity expected[4] = { n[0], n[1], n[3], n[4] };
    zox_tst_check(zox_tst_nodes_begun_count == 4, "Begin was added [%i] times, not once for each of 4 nodes", zox_tst_nodes_begun_count);
    for (int i = 0; i < 4 && i < zox_tst_nodes_begun_count; i++) {
        zox_tst_check(zox_tst_nodes_begun[i] == expected[i], "Begin [%i] had node [%s] current", i,
            zox_tst_nodes_begun[i] ? zox_getn(zox_tst_nodes_begun[i]) : "nothing");
    }
    if (zox_tst_nodes_observer) {
        zox_delete(zox_tst_nodes_observer);
    }
    if (zox_valid(zox_tst_nodes_process2)) {
        zox_delete(zox_tst_nodes_process2);
    }
    for (int i = 0; i < 5; i++) {
        zox_delete(n[i]);
    }
}

void zox_tsts_nodes() {
    zox_tst_add("node_graphs", zox_tst_node_graphs_ready, zox_tst_node_graphs_check, 30);
    zox_tst_set_setup("node_graphs", NULL, 1);
}
