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
zoxc_float3(HomePosition);       // where an npc lives (its spawn), wandering past the leash turns it home
zoxc_double(AttackAt);           // its next swing not before, backing off from its threat till near it
zox_tag(FollowTarget);     // npc -> the entity it follows

// its settings, each its own tag (a setting's observer finds it by it, not its name)
zox_tag(SettingNoHumans);

void zox_components_npcs(ecs* world) {
    zoxd_tag(SettingNoHumans);

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
    zoxd_double(AttackAt);
    zoxd_nf_link(FollowTarget);
}