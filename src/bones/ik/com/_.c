zoxc_float3(IKTarget);  // on a shoulder bone: the point in the world its hand reaches for
zoxc_float(IKWeight);   // on a shoulder bone: 0 its animation, 1 the hand on IKTarget
zoxc_float4(IKTargetRotation);   // on a shoulder bone, optional: the hand turns to it (an XR controller's), its rest down along its forward (-z)
zox_tag(IKHeld);        // on a shoulder bone: a driver holds its target (XR hands), a punch leaves it be

void zox_components_ik(ecs* world) {
    zoxd_float3(IKTarget);
    zoxd_float(IKWeight);
    zoxd_float4(IKTargetRotation);
    zoxd_tag(IKHeld);
}
