zox_tag(EditorElement);
zox_tag(InspectorUI);
zox_tag(InspectorLabel);
zoxc_state(InspectorDirty);
zox_tag(HierarchyUI);
zoxc_state(HierarchyUIDirty);
zox_tag(MaxSystemTimeLabel);
zox_tag(FPSDisplay);
zox_tag(GameDebugLabel);
zox_tag(Profiler);
zoxc_double(FPSDisplayTicker);
zoxc(DebugLabelData, DebugLabelEvent);

void zox_components_editor(ecs* world) {
    zoxd_tag(EditorElement);
    zoxd_tag(InspectorLabel);
    zoxd_tag(InspectorUI);
    zoxd_tag(HierarchyUI);
    zoxd_state(HierarchyUIDirty);
    zoxd_state(InspectorDirty);
    zoxd_tag(MaxSystemTimeLabel);
    zoxd_tag(FPSDisplay);
    zoxd_tag(GameDebugLabel);
    zoxd_tag(Profiler);
    zoxd_double(FPSDisplayTicker);
    zoxd(DebugLabelData);
}
