zox_tag(EditorElement);
zox_tag(HierarchyUI);
zox_tag(InspectorLabel);
zoxc_state(HierarchyUIDirty);
zoxc_entity(EditorTarget);

void define_components_editor(ecs* world) {
    zoxd_tag(EditorElement);
    zoxd_tag(HierarchyUI);
    zoxd_tag(InspectorLabel);
    zoxd_state(HierarchyUIDirty);
    zoxd_entity(EditorTarget);
}
