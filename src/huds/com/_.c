zox_tag(MenuMain);
zox_tag(MenuOptions);
zox_tag(MenuPaused);
zox_tag(MenuPlay);
zox_tag(MenuPlayTouch);
zox_tag(MenuStart);
zox_tag(MenuLoad);
zox_tag(MenuTerminal);
zox_tag(MenuRealm);
zox_tag(MenuNewRealm);
zox_tag(Crosshair);
zox_tag(RespawnUI);
zoxc_byte(HitType);
// Taskbar
zox_tag(Taskbar);
zoxc_entity(TaskbarWindowID);
zoxc_entity(TaskbarToggleLink);
zoxc_entity(IconOverlayLink);

void define_components_game_ui(ecs *world) {
    zoxd_tag(MenuMain);
    zoxd_tag(MenuOptions);
    zoxd_tag(MenuPaused);
    zoxd_tag(MenuPlay);
    zoxd_tag(MenuPlayTouch);
    zoxd_tag(MenuStart);
    zoxd_tag(MenuLoad);
    zoxd_tag(MenuTerminal);
    zoxd_tag(MenuRealm);
    zoxd_tag(MenuNewRealm);
    zoxd_tag(Crosshair);
    zoxd_tag(RespawnUI);
    zoxd_byte(HitType);
    zoxd_entity(IconOverlayLink);
    // Taskbar
    zoxd_tag(Taskbar);
    zoxd_entity(TaskbarWindowID);
    zoxd_entity(TaskbarToggleLink);
}
