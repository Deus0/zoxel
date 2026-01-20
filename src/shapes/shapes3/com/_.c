zox_tag(Shape3);
zox_tag(Cube);
zoxc_byte(Radius);
zoxc_byte3(Shape3Position);
zoxc_byte3(Shape3Size);

void define_components_shapes3(ecs* world) {
    zoxd_tag(Shape3);
    zoxd_tag(Cube);
    zoxd_byte(Radius);
    zoxd_byte3(Shape3Position);
    zoxd_byte3(Shape3Size);
}