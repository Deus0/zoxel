#include "bluenode.c"
// #include "blueprint.c"
#include "bluelink.c"
// entity prefab_blueprint;
entity prefab_bluenode;
entity prefab_bluelink;

void spawn_prefabs_blueprints(ecs *world) {
    prefab_bluenode = spawn_prefab_bluenode(world, prefab_node);
    prefab_bluelink = spawn_prefab_bluelink(world, prefab_node_link);
    // prefab_blueprint = spawn_prefab_blueprint(world, prefab_node_tree);
}
