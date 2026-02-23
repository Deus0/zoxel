zox_tag(MenuBody);
zox_tag(BodyItem);
zox_tag(BodyModel);
zox_tag(BodyUnique);
zoxc_state(BodyDirty);
zoxc_byte(SlotType);
zoxc_entities(BodyLinks);
zoxc_entity(AttachLink);
zoxc_entities_weak(PartLinks);

void define_components_bodys(ecs* world) {
    zoxd_tag(MenuBody);
    zoxd_tag(BodyItem);
    zoxd_tag(BodyModel);
    zoxd_tag(BodyUnique);
    zoxd_state(BodyDirty);
    zoxd_entities(BodyLinks);
    zoxd_byte(SlotType);
    zoxd_entity(AttachLink);
    zoxd_entities_weak(PartLinks);
}
