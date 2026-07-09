zox_tag(SunnyChunk);
zox_tag(BottomChunk);
zoxc_byte(RefreshLights);
zoxc_octree(LightNode, byte, 0);
new_octree_function_set(LightNode);
create_node_getter(LightNode);
create_octree_get_nearby(LightNode);
create_octree_reducer(LightNode);
create_octree_optimizer(LightNode);
create_optimize_by_max(LightNode);
create_node_setreduce(LightNode);
create_node_neighbor(LightNode);
create_octree_line_debugger(LightNode);
create_octree_line_debugger_compare(LightNode, VoxelNode);
zoxc_byte(LightNodeDepth);
zoxc_state(LightNodeDirty);
zoxc_queue(SunlightQueue, SunlightUpdate, 1)
zoxc_queue(LightQueue, LightUpdate, 1)
zoxc_queue(DarkQueue, DarkUpdate, 1)

void define_components_lights3(ecs *world) {
    zoxd_tag(SunnyChunk);
    zoxd_tag(BottomChunk);
    zoxd_octree(LightNode); // zoxd_node
    zoxd_byte(LightNodeDepth);
    zoxd_state(LightNodeDirty);
    zoxd_queue(SunlightQueue);
    zoxd_queue(LightQueue);
    zoxd_queue(DarkQueue);
    zoxd_byte(RefreshLights);
}
