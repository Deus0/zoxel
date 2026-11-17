zoxc_float3(IKTarget);  // on a shoulder bone: the point in the world its hand reaches for
zoxc_float(IKWeight);   // on a shoulder bone: 0 its animation, 1 the hand on IKTarget
zox_tag(IKHeld);        // on a shoulder bone: a driver holds its target (XR hands), a punch leaves it be

void zox_components_ik(ecs* world) {
    zoxd_float3(IKTarget);
    zoxd_float(IKWeight);
    zoxd_tag(IKHeld);
}
