zox_tag(Actionbar);
// zox_tag(ActionbarUI);
zox_tag(MenuActions);
zox_tag(ActionFrame);
zox_tag(IconAction);
zox_tag(ActionSwapped);    // a player swapped its action: on_action_swapped plays its sound, removes it
zox_tag(Castbar);           // a character's cast (its action's warmup) filling, above its actionbar (CharacterLink)
zox_tag(CastbarText);       // its text: the cast's name
zoxc_entity(CastAction);    // on a castbar: the action it shows, its name set when it changes

void zox_components_actions_ui(ecs* world) {
    zoxd_tag(Actionbar);
    // zoxd_tag(ActionbarUI);
    zoxd_tag(MenuActions);
    zoxd_tag(ActionFrame);
    zoxd_tag(IconAction);
    zoxd_nf_tag(ActionSwapped);
    zoxd_tag(Castbar);
    zoxd_tag(CastbarText);
    zoxd_entity(CastAction);
}
