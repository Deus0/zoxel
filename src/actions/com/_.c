zox_tag(Action);
zoxc_entity(ActiveAction);
zoxc_state(ActiveActionDirty);
zoxc_byte(ActionIndex);
zox_tag(HeldAction);

void zox_define_components_actions(ecs* world) {
    zoxd_tag(Action);
    zoxd_entity(ActiveAction);
    zoxd_state(ActiveActionDirty);
    zoxd_byte(ActionIndex);
    zoxd_tag(HeldAction);
}
