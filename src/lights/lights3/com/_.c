zox_tag(SunnyChunk);
zox_tag(BottomChunk);
zoxc_octree_basic(LightNode, byte, darklight);
zoxc(LightNodeLock, spinlock);

create_octree_setters(LightNode);
create_octree_accessors(LightNode);
create_octree_get_nearby(LightNode);
create_octree_reducer(LightNode, 0);
create_octree_optimizer(LightNode, 0);
create_optimize_by_max(LightNode);
create_node_neighbor(LightNode);
create_octree_line_debugger(LightNode);
create_octree_line_debugger_compare(LightNode, VoxelNode);
zoxc_byte(LightNodeDepth);
zoxc_state(LightNodeDirty);

zoxc_queue(SunlightQueue, SunlightUpdate, 1);
zoxc_queue(LightQueue, LightUpdate, 1);
zoxc_queue(DarkQueue, DarkUpdate, 1);
zoxc_queue_remove(SunlightQueue, SunlightUpdate);
zoxc_queue_remove(LightQueue, LightUpdate);
zoxc_queue_remove(DarkQueue, DarkUpdate);

void define_components_lights3(ecs *world) {
    zoxd_nf_tag(SunnyChunk);
    zoxd_nf_tag(BottomChunk);
    zoxd_octree(LightNode); // zoxd_node
    zoxd(LightNodeLock);
    zoxd_byte(LightNodeDepth);
    zoxd_state(LightNodeDirty);
    zoxd_queue(SunlightQueue);
    zoxd_queue(LightQueue);
    zoxd_queue(DarkQueue);
}
