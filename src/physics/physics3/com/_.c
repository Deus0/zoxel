zox_tag(Rolling3D);              // rolls over the ground without slipping, see rolling3_system
zoxc_byte(InitializePhysics3D);
zoxc_float3(Velocity3D);
zoxc_float3(Acceleration3D);
zoxc_float3(Omega3D);
zoxc_float3(Alpha3D);
// move to new sub module - Forces
zoxc_float3(Gravity3D);
zoxc_byte(Flying);
zoxc_float3(LastPosition3D);
zoxc_float3(LastUnstuck3);

void zox_components_physics3(ecs *world) {
    zoxd_tag(Rolling3D);
    zoxd_byte(InitializePhysics3D);
    zoxd_float3(Velocity3D);
    zoxd_float3(Acceleration3D);
    zoxd_float3(Omega3D);
    zoxd_float3(Alpha3D);
    zoxd_float3(Gravity3D);
    zoxd_float3(LastPosition3D);
    zoxd_float3(LastUnstuck3);
}