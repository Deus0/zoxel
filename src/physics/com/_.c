zox_tag(Moveable);
zox_tag(Frictioned);
zoxc_byte(DisableMovement);
zoxc_byte(DisableGravity);
zoxc_byte(CollisionDisabled);

void zox_define_components_physics(ecs* world) {
    zoxd_tag(Moveable);
    zoxd_tag(Frictioned);
    zoxd_byte(DisableMovement);
    zoxd_byte(DisableGravity);
    zoxd_byte(CollisionDisabled);
}
