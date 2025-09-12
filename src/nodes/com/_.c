zox_tag(Node);
zox_tag(Nodelink);
zox_tag(Nodeleaf);
zox_tag(Nodetree);
zoxc_entity(NodeParent);
zoxc_entity(NodeNext);

void define_components_nodes(ecs* world) {
    zoxd_tag(Node);
    zoxd_tag(Nodelink);
    zoxd_tag(Nodeleaf);
    zoxd_tag(Nodetree);
    zoxd_entity(NodeParent);
    zoxd_entity(NodeNext);
}