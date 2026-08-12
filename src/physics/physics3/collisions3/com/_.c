zox_tag(SphereCollider);
zoxc_byte(Collision);       // a flag for sides collided with
zoxc_float3(CollisionDistance);
zoxc_float(BasicCollider);
zoxc_float(SphereRadius);
zoxc_function(OverlapEvent, byte, ecs*, entity, entity);

void define_components_collisions3(ecs* world) {
    zoxd_tag(SphereCollider);
    zoxd_byte(Collision);
    zoxd_float(BasicCollider);
    zoxd_float(SphereRadius);
    zoxd_float3(CollisionDistance);
    zoxd(OverlapEvent);
}
