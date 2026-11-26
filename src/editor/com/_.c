zox_tag(HierarchySlot);
zox_tag(EditorElement);
zox_tag(EditorMenubarDirty);
zox_tag(EditorWindowMenu);
zox_tag(InspectorUI);
zox_tag(InspectorLabel);
zox_tag(InspectorDirty);
zox_tag(HierarchyUI);
zox_tag(FileWindowUI);
zox_tag(MaxSystemTimeLabel);
zox_tag(FPSDisplay);
zox_tag(SettingFPSDisplay);     // the "FPS" setting (Video): the fps display top right, in every build
zox_tag(GameDebugLabel);
zox_tag(Profiler);
zoxc_double(FPSDisplayTicker);
zoxc(DebugLabelData, DebugLabelEvent);

void zox_components_editor(ecs* world) {
    zoxd_tag(EditorElement);
    zoxd_nf_tag(EditorMenubarDirty);
    zoxd_tag(EditorWindowMenu);
    zoxd_tag(InspectorLabel);
    zoxd_tag(InspectorUI);
    zoxd_tag(HierarchyUI);
    zoxd_tag(HierarchySlot);
    zoxd_tag(FileWindowUI);
    zoxd_nf_tag(InspectorDirty);
    zoxd_tag(MaxSystemTimeLabel);
    zoxd_tag(FPSDisplay);
    zoxd_tag(SettingFPSDisplay);
    zoxd_tag(GameDebugLabel);
    zoxd_tag(Profiler);
    zoxd_double(FPSDisplayTicker);
    zoxd(DebugLabelData);
}
