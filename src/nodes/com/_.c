// Types
zox_tag(Node);
zox_tag(Nodelink);
zox_tag(Nodeleaf);
zox_tag(Nodetree);
zox_tag(NodeRun);
zoxc_byte(NodeType);
// Links
zox_tag(NodeLink);
zox_tag(CurrentNodeLink);
zox_tag(NextNodeLink);
zoxc_entity(NodeStartLink);
zoxc_entity(NodeParent);
zoxc_entity(NodeNext);
// Events
zoxc_state(NodeBegin);
zoxc_state(NodeEnd);
zoxc_state(NodetreeBegin);
zoxc_state(NodetreeEnd);
zoxc_listener(NodeBeginEvent, 4, ecs*, entity, entity);
// Conditional Nodes
zox_tag(NodeTrue);
zox_tag(NodeFalse);

void zox_components_nodes(ecs* world) {
    zoxd_tag(Node);
    zoxd_tag(Nodelink);
    zoxd_tag(Nodeleaf);
    zoxd_tag(Nodetree);
    zoxd_tag(NodeRun);
    zoxd_byte(NodeType);
    zoxd_state(NodeBegin);
    zoxd_state(NodeEnd);
    zoxd_state(NodetreeBegin);
    zoxd_state(NodetreeEnd);
    zoxd_entity(NodeParent);
    zoxd_entity(NodeNext);
    zoxd_entity(NodeStartLink);
    zoxd(NodeBeginEvent);
    // Links
    zoxd_nf_tag(NodeLink);
    zoxd_nf_tag(CurrentNodeLink);
    zoxd_nf_tag(NextNodeLink);
    zoxd_tag_event(NodeTrue);
    zoxd_tag_event(NodeFalse);
}
