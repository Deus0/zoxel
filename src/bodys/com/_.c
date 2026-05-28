zox_tag(MenuBody);
zox_tag(Body);
zox_tag(BodyPart);
zox_tag(BodyItem);
zox_tag(BodyModel);
zox_tag(BodyUnique);
zoxc_state(BodyDirty);
zoxc_byte(SlotType);
zoxc_byte(SlotAnchor);
zoxc_byte3(BodySize);
zoxc_byte3(PartPosition);
zoxc_byte3(PartSize);
zoxc_int3(PartOffset); // for now
zox_tag(Head);
zox_tag(Hand);
zox_tag(Shoulder);

void define_components_bodys(ecs* world) {
    zoxd_tag(MenuBody);
    zoxd_tag(Body);
    zoxd_tag(BodyPart);
    zoxd_tag(BodyItem);
    zoxd_tag(BodyModel);
    zoxd_tag(BodyUnique);
    zoxd_state(BodyDirty);
    zoxd_byte(SlotType);
    zoxd_byte(SlotAnchor);
    zoxd_byte3(BodySize);
    zoxd_byte3(PartPosition);
    zoxd_byte3(PartSize);
    zoxd_int3(PartOffset);
    zoxd_tag(Head);
    zoxd_tag(Hand);
    zoxd_tag(Shoulder);
}
