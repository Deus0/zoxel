// Types
zox_tag(Node);
zox_tag(NodeRun);
zox_tag(Nodeleaf);
zox_tag(Nodetree);
// TODO: Replace this with tags in the modules theyre used
zoxc_byte(NodeType);
// Conditional Nodes
zox_tag(NodeTrue);
zox_tag(NodeFalse);
// Links
zox_tag(NodeLink);          // links nodes to nodes and runner to first node
zox_tag(CurrentNodeLink);   // Current node runner is at
zox_tag(NextNodeLink);      // Next node runner will move to

void zox_components_nodes(ecs* world) {
    zoxd_tag(Node);
    zoxd_tag(NodeRun);
    zoxd_tag(Nodeleaf);
    zoxd_tag(Nodetree);
    // Proprties
    zoxd_tag_event(NodeTrue);
    zoxd_tag_event(NodeFalse);
    zoxd_byte(NodeType);
    // Links
    zoxd_nf_tag(NodeLink);
    zoxd_nf_tag(CurrentNodeLink);
    zoxd_nf_tag(NextNodeLink);
}
