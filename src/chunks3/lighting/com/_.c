zoxc_node(LightNode, byte, 0)
zoxc_node_helper(LightNode, byte)
zoxc_byte(LightNodeDirty);
zoxc_queue(LightNodeQueue, LightNodeUpdate, 1)

void define_components_lighting3(ecs *world) {
    zoxd_node(LightNode);
    zoxd_byte(LightNodeDirty);
    zoxd_queue(LightNodeQueue);
}