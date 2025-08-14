zox_tag(SunnyChunk);

zoxc_octree_fun1(LightNode, byte, 0)
create_node_setter(LightNode)
create_node_getter(LightNode)
create_node_reducer(LightNode)
create_node_setreduce(LightNode)
create_node_neighbor(LightNode)

zoxc_byte(LightNodeDepth);
zoxc_byte(LightNodeDirty);
zoxc_queue(LightNodeQueue, LightNodeUpdate, 1)
zoxc_queue(SunlightQueue, SunlightUpdate, 1)

void define_components_lighting3(ecs *world) {
    zoxd_tag(SunnyChunk);
    zoxd_node(LightNode);
    zoxd_byte(LightNodeDepth);
    zoxd_byte(LightNodeDirty);
    zoxd_queue(LightNodeQueue);
    zoxd_queue(SunlightQueue);
}