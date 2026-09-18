zox_tag(MenuBody);
zox_tag(BodyFrame);
zox_tag(Body);
zox_tag(BodyPart);
zox_tag(BodyItem);
zox_tag(BodyModel);
zox_tag(BodyUnique);
zox_tag(BodySlot);
zox_tag(ArmPart);
zox_tag(Head);
zox_tag(Hand);
zox_tag(Hips);
zox_tag(Shoulder);
zox_tag(Thigh);
zoxc_byte(BodyDirty);
zoxc_byte3(BodySize);
zoxc_byte3(PartPosition);
zoxc_byte3(PartSize);
zoxc_int3(PartOffset); // for now

#define zox_generate_body_start 1
#define zox_generate_body_combine 1
#define zox_generate_body_bones 2
#define zox_generate_body_end 0

void zox_components_bodys(ecs* world) {
    zoxd_tag(MenuBody);
    zoxd_tag(BodyFrame);
    zoxd_tag(Body);
    zoxd_tag(BodyPart);
    zoxd_tag(BodyItem);
    zoxd_tag(BodyModel);
    zoxd_tag(BodyUnique);
    zoxd_tag(BodySlot);
    zoxd_tag(ArmPart);
    zoxd_tag(Head);
    zoxd_tag(Hand);
    zoxd_tag(Hips);
    zoxd_tag(Shoulder);
    zoxd_tag(Thigh);
    zoxd_byte(BodyDirty);
    zoxd_byte3(BodySize);
    zoxd_byte3(PartPosition);
    zoxd_byte3(PartSize);
    zoxd_int3(PartOffset);
}
