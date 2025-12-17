zoxc_byte(MenuType);

// Game Menus
// TODO: Remove use of tags for this, just use an ID
zox_tag(MenuMain);
zox_tag(MenuOptions);
zox_tag(MenuPaused);
zox_tag(MenuGame);
zox_tag(MenuGameTouch);
zox_tag(MenuStart);
zox_tag(MenuLoad);
zox_tag(MenuTerminal);

zox_tag(FPSDisplay);
zox_tag(QuadsCountLabel);
zox_tag(GameDebugLabel);
zox_tag(Profiler);
zox_tag(Taskbar);
zox_tag(MenuNewRealm);
zoxc_byte(HitType);
zoxc_entity(TaskbarButton);
zoxc_int(QuadsCount);
zoxc_double(FPSDisplayTicker);
zoxc(DebugLabelData, DebugLabelEvent);
// #include "debug_label_data.c"

void define_components_game_ui(ecs *world) {
    zoxd_byte(MenuType);

    zoxd_tag(MenuMain);
    zoxd_tag(MenuOptions);
    zoxd_tag(MenuPaused);
    zoxd_tag(MenuGame);
    zoxd_tag(MenuGameTouch);
    zoxd_tag(MenuStart);
    zoxd_tag(MenuLoad);
    zoxd_tag(MenuTerminal);

    zoxd_tag(FPSDisplay);
    zoxd_tag(QuadsCountLabel);
    zoxd_tag(GameDebugLabel);
    zoxd_tag(Profiler);
    zoxd_tag(Taskbar);
    zoxd_tag(MenuNewRealm);;
    zoxd_entity(TaskbarButton);
    zoxd_byte(HitType);
    zoxd_int(QuadsCount);
    zoxd_double(FPSDisplayTicker);
    zoxd(DebugLabelData);
}