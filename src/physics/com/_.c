zox_tag(Moveable);
zox_tag(Frictioned);
zox_tag(NoClip);
zox_tag(DisableGravity);
zox_tag(DisableMovement);
zox_tag(UprightCollider);    // hits terrain as an upright box that doesn't turn, its footprint under its arms (terrain collisions)
zoxc_byte(CollisionDisabled);
zoxc_byte(Grounded); // use for friction - todo: use bit operations to check what sides cause friction and apply like that

void zox_components_physics(ecs* world) {
    zoxd_tag(Moveable);
    zoxd_tag(Frictioned);
    zoxd_tag(NoClip);
    zoxd_tag(DisableGravity);
    zoxd_tag(DisableMovement);
    zoxd_tag(UprightCollider);
    zoxd_byte(CollisionDisabled);
    zoxd_byte(Grounded);
}
