zoxc_float3(IKTarget);  // on a shoulder bone: the point in the world its hand reaches for (a thigh's: its foot)
zoxc_float(IKWeight);   // on a shoulder bone: 0 its animation, 1 the hand on IKTarget (a thigh's: its foot)
zoxc_float4(IKTargetRotation);   // on a shoulder bone, optional: the hand turns to it (an XR controller's), its rest down along its forward (-z)
zox_tag(IKHeld);        // on a shoulder bone: a driver holds its target (XR hands), a punch leaves it be
zox_tag(PlantFeet);     // on a skeleton: its feet stay planted in the world as the body shifts, stepping when far (sys/feet.c)
zoxc_float3(FootPlant);     // on a thigh bone: where its foot is planted in the world
zoxc_float3(FootStepFrom);  // on a thigh bone: where its foot's step started
zoxc_float(FootStep);       // on a thigh bone: its foot's step, 0 planted, rising to 1 as it lands
zoxc_float4(SpineLean);     // on a skeleton: its chest turned about its hips (the upper body leaning), the hips upright
zoxc_float(HipsDrop);       // on a skeleton: its hips lowered (crouching: the knees bend, the feet stay on the ground)
zoxc_float3(RestPosition3D);    // on a chest bone: its LocalPosition3D unleaned, undropped (sys/spine.c)

void zox_components_ik(ecs* world) {
    zoxd_float3(IKTarget);
    zoxd_float(IKWeight);
    zoxd_float4(IKTargetRotation);
    zoxd_tag(IKHeld);
    zoxd_tag(PlantFeet);
    zoxd_float3(FootPlant);
    zoxd_float3(FootStepFrom);
    zoxd_float(FootStep);
    zoxd_float4(SpineLean);
    zoxd_float(HipsDrop);
    zoxd_float3(RestPosition3D);
}
