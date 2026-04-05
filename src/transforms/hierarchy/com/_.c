zoxc_byte(ChildIndex);
zoxc_entities(Children)
zoxc_child(ParentLink, Children);
zoxc_entity(OldParentLink);

void define_components_hierarchy(ecs* world) {
    zoxd_byte(ChildIndex);
    zoxd_entities(Children);
    zoxd_child(ParentLink);
    zoxd_entity(OldParentLink);
}
