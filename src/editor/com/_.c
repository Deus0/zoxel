zox_tag(EditorElement);
// zox_tag(HierarchyUI);
zox_tag(InspectorLabel);
//zoxc_entity(EditorTarget);
//zoxc_entity(EditorTargetID);
zoxc_state(HierarchyUIDirty);
zoxc_state(InspectorDirty);

void define_components_editor(ecs* world) {
    zoxd_tag(EditorElement);
    // zoxd_tag(HierarchyUI);
    zoxd_tag(InspectorLabel);
    //zoxd_entity(EditorTarget);
    //zoxd_entity(EditorTargetID);
    zoxd_state(HierarchyUIDirty);
    zoxd_state(InspectorDirty);
}
