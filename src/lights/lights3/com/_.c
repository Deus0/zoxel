zox_tag(SunnyChunk);
zox_tag(BottomChunk);
zoxc_octree_basic(LightOctree, byte, darklight);

create_octree_setters(LightOctree);
create_octree_accessors(LightOctree);
create_octree_get_nearby(LightOctree);
create_octree_get_nearby_max(LightOctree);
create_octree_reducer(LightOctree);
create_octree_optimizer(LightOctree, 0);
create_optimize_by_max(LightOctree);
create_node_neighbor(LightOctree);
create_octree_line_debugger(LightOctree);
create_octree_line_debugger_compare(LightOctree, VoxelNode);
zoxc_byte(LightOctreeDepth);
zoxc_state(LightOctreeDirty);

zoxc_queue(SunlightQueue, SunlightUpdate, 1);
zoxc_queue(LightQueue, LightUpdate, 1);
zoxc_queue(DarkQueue, DarkUpdate, 1);
zoxc_queue_remove(SunlightQueue, SunlightUpdate);
zoxc_queue_remove(LightQueue, LightUpdate);
zoxc_queue_remove(DarkQueue, DarkUpdate);

void zox_components_lights3(ecs *world) {
    // NOTE: Sunny used in system filter
    zoxd_tag(SunnyChunk);
    // Property
    zoxd_nf_tag(BottomChunk);
    zoxd_octree(LightOctree); // zoxd_node
    zoxd_byte(LightOctreeDepth);
    zoxd_state(LightOctreeDirty);
    zoxd_queue(SunlightQueue);
    zoxd_queue(LightQueue);
    zoxd_queue(DarkQueue);
}
