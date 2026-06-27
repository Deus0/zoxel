zox_tag(EditorElement);
zox_tag(InspectorUI);
zox_tag(InspectorLabel);
zoxc_state(InspectorDirty);
zox_tag(HierarchyUI);
zoxc_state(HierarchyUIDirty);
zox_tag(MaxSystemTimeLabel);

void define_components_editor(ecs* world) {
    zoxd_tag(EditorElement);
    zoxd_tag(InspectorLabel);
    zoxd_tag(InspectorUI);
    zoxd_tag(HierarchyUI);
    zoxd_state(HierarchyUIDirty);
    zoxd_state(InspectorDirty);
    zoxd_tag(MaxSystemTimeLabel);
}
