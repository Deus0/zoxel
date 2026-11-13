zox_tag(Npc);
zox_tag(Jumper);
zox_tag(Wanderer);
zox_tag(Coward);
zoxc_byte(Behaviour);
zoxc_byte(DefaultBehaviour);
zoxc_byte(MoveForwards);
zoxc_byte(RotateTowards);
zoxc_float(MoveSpeed);
zoxc_float(MoveToBuffer);
zoxc_float3(TargetPosition);
zoxc_float3(ThreatPosition);
zoxc_float3(WanderDirection);
zoxc_float3(HomePosition);       // where an npc lives (its spawn), wandering past the leash turns it home
zox_tag(FollowTarget);     // npc -> the entity it follows

void zox_components_npcs(ecs* world) {
    zoxd_tag(Npc);
    zoxd_tag(Jumper);
    zoxd_tag(Wanderer);
    zoxd_tag(Coward);
    zoxd_float3(HomePosition);
    zoxd_byte(Behaviour);
    zoxd_byte(DefaultBehaviour);
    zoxd_byte(MoveForwards);
    zoxd_byte(RotateTowards);
    zoxd_float(MoveSpeed);
    zoxd_float(MoveToBuffer);
    zoxd_float3(TargetPosition);
    zoxd_float3(ThreatPosition);
    zoxd_float3(WanderDirection);
    zoxd_nf_link(FollowTarget);
}