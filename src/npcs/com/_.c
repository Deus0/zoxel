zox_tag(Npc);
zox_tag(Jumper);
zox_tag(Wanderer);
zox_tag(Coward);
zoxc_byte(Behaviour);
zoxc_byte(DefaultBehaviour);
zoxc_byte(MoveForwards);
zoxc_byte(RotateTowards);
zoxc_float(MoveSpeed);
zoxc_float3(TargetPosition);
zoxc_float3(ThreatPosition);
zoxc_float3(WanderDirection);

void define_components_npcs(ecs* world) {
    zoxd_tag(Npc);
    zoxd_tag(Jumper);
    zoxd_tag(Wanderer);
    zoxd_tag(Coward);
    zoxd_byte(Behaviour);
    zoxd_byte(DefaultBehaviour);
    zoxd_byte(MoveForwards);
    zoxd_byte(RotateTowards);
    zoxd_float(MoveSpeed);
    zoxd_float3(TargetPosition);
    zoxd_float3(ThreatPosition);
    zoxd_float3(WanderDirection);
}