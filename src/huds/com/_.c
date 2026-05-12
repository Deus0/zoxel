zoxc_byte(MenuType);    // TODO: Remove this, use tags again lol
zox_tag(MenuMain);
zox_tag(MenuOptions);
zox_tag(MenuPaused);
zox_tag(MenuPlay);
zox_tag(MenuPlayTouch);
zox_tag(MenuStart);
zox_tag(MenuLoad);
zox_tag(MenuTerminal);
zox_tag(MenuRealm);
zox_tag(FPSDisplay);
zox_tag(QuadsCountLabel);
zox_tag(GameDebugLabel);
zox_tag(Profiler);
zox_tag(MenuNewRealm);
zox_tag(Crosshair);
zoxc_byte(HitType);
zoxc_int(QuadsCount);
zoxc_double(FPSDisplayTicker);
zoxc(DebugLabelData, DebugLabelEvent);
// Taskbar
zox_tag(Taskbar);
zoxc_entity(TaskbarToggleLink);
zoxc_entity(IconOverlayLink);

// #include "debug_label_data.c"

void define_components_game_ui(ecs *world) {
    zoxd_byte(MenuType);
    zoxd_tag(MenuMain);
    zoxd_tag(MenuOptions);
    zoxd_tag(MenuPaused);
    zoxd_tag(MenuPlay);
    zoxd_tag(MenuPlayTouch);
    zoxd_tag(MenuStart);
    zoxd_tag(MenuLoad);
    zoxd_tag(MenuTerminal);
    zoxd_tag(MenuRealm);
    zoxd_tag(FPSDisplay);
    zoxd_tag(QuadsCountLabel);
    zoxd_tag(GameDebugLabel);
    zoxd_tag(Profiler);
    zoxd_tag(MenuNewRealm);
    zoxd_tag(Crosshair);
    zoxd_byte(HitType);
    zoxd_int(QuadsCount);
    zoxd_double(FPSDisplayTicker);
    zoxd(DebugLabelData);
    zoxd_entity(IconOverlayLink);
    // Taskbar
    zoxd_tag(Taskbar);
    zoxd_entity(TaskbarToggleLink);
}
