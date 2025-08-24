// zox_tag(Player3D)
zox_tag(Character3Player);
zoxc_float3(Movement3);

void define_components_controllers3D(ecs_world_t *world) {
    // zoxd_tag(Player3D)
    zoxd_tag(Character3Player);
    zox_define_component_float3(Movement3);
}