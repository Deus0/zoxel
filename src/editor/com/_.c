zox_tag(EditorElement);
zox_tag(InspectorUI);
zox_tag(InspectorLabel);
zoxc_state(InspectorDirty);
zox_tag(HierarchyUI);
zoxc_state(HierarchyUIDirty);
//zoxc_entity(EditorTarget);
//zoxc_entity(EditorTargetID);

void define_components_editor(ecs* world) {
    zoxd_tag(EditorElement);
    // zoxd_tag(HierarchyUI);
    zoxd_tag(InspectorLabel);
    zoxd_tag(InspectorUI);
    //zoxd_entity(EditorTarget);
    //zoxd_entity(EditorTargetID);
    zoxd_tag(HierarchyUI);
    zoxd_state(HierarchyUIDirty);
    zoxd_state(InspectorDirty);
}
