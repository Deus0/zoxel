zox_tag(Actionbar);
// zox_tag(ActionbarUI);
zox_tag(MenuActions);
zox_tag(ActionFrame);
zox_tag(IconAction);
zox_tag(ActionSwapped);    // a player swapped its action: on_action_swapped plays its sound, removes it

void zox_components_actions_ui(ecs* world) {
    zoxd_tag(Actionbar);
    // zoxd_tag(ActionbarUI);
    zoxd_tag(MenuActions);
    zoxd_tag(ActionFrame);
    zoxd_tag(IconAction);
    zoxd_nf_tag(ActionSwapped);
}
