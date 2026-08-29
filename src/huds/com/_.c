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
zox_tag(LoadUI);
zoxc_byte(HitType);
// Taskbar
zox_tag(Taskbar);
zoxc_entity(TaskbarWindowID);
zoxc_entity(TaskbarToggleLink);
zoxc_entity(IconOverlayLink);

void define_components_game_ui(ecs *world) {
    zoxd_nf_tag(MenuMain);
    zoxd_nf_tag(MenuOptions);
    zoxd_nf_tag(MenuPaused);
    zoxd_nf_tag(MenuPlay);
    zoxd_nf_tag(MenuPlayTouch);
    zoxd_nf_tag(MenuStart);
    zoxd_nf_tag(MenuLoad);
    zoxd_nf_tag(MenuTerminal);
    zoxd_nf_tag(MenuRealm);
    zoxd_nf_tag(MenuNewRealm);
    zoxd_nf_tag(Crosshair);
    zoxd_nf_tag(RespawnUI);
    zoxd_nf_tag(LoadUI);
    zoxd_nf_tag(Taskbar);
    // Others
    zoxd_byte(HitType);
    zoxd_entity(IconOverlayLink);
    zoxd_entity(TaskbarWindowID);
    zoxd_entity(TaskbarToggleLink);
}
