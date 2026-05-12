zox_tag(MenuBody);
zox_tag(Body);
zox_tag(BodyItem);
zox_tag(BodyModel);
zox_tag(BodyUnique);
zoxc_state(BodyDirty);
zoxc_byte(SlotType);
zoxc_entity(AttachLink);
zoxc_entities(PartLinks);
// zoxc_entitiesw(PartLinks);
zoxc_byte3(BodySize);
zoxc_byte3(PartSize);
zoxc_byte3(PartPosition);

void define_components_bodys(ecs* world) {
    zoxd_tag(MenuBody);
    zoxd_tag(Body);
    zoxd_tag(BodyItem);
    zoxd_tag(BodyModel);
    zoxd_tag(BodyUnique);
    zoxd_state(BodyDirty);
    zoxd_byte(SlotType);
    zoxd_entity(AttachLink);
    zoxd_entities(PartLinks);
    zoxd_byte3(BodySize);
    zoxd_byte3(PartSize);
    zoxd_byte3(PartPosition);
}
