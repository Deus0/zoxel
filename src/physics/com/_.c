zox_tag(Moveable);
zox_tag(Frictioned);
zox_tag(NoClip);
zox_tag(DisableGravity);
zox_tag(DisableMovement);
zoxc_byte(CollisionDisabled);

void zox_define_components_physics(ecs* world) {
    zoxd_tag(Moveable);
    zoxd_tag(Frictioned);
    zoxd_tag(NoClip);
    zoxd_tag(DisableGravity);
    zoxd_tag(DisableMovement);
    zoxd_byte(CollisionDisabled);
}
