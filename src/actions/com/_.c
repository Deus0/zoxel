zox_tag(Action);
zoxc_entity(ActiveAction);
zoxc_state(ActiveActionDirty);
zoxc_entity(ActionLink);
// zoxc_entitiesw(ActionLinks);
zoxc_byte(ActionIndex);

void zox_define_components_actions(ecs* world) {
    zoxd_tag(Action);
    zoxd_entity(ActiveAction);
    zoxd_state(ActiveActionDirty);
    zoxd_entity(ActionLink);
    // zoxd_entitiesw(ActionLinks);
    zoxd_byte(ActionIndex);
}
