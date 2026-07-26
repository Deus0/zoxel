zoxc_float2(Position2);
zoxc_float(Rotation2);
zoxc_float2(Scale2);
zoxc_float2(LocalPosition2);
zoxc_float(LocalRotation2);
zoxc_float2(LocalScale2);
// Lines
typedef struct {
    float2 start;
    float2 end;
} LinePoints2;
zoxc_custom(LinePoints2);
typedef struct {
    float2 start;
    float2 end;
} LocalLinePoints2;
zoxc_custom(LocalLinePoints2);

void zox_define_components_transforms2(ecs* world) {
    zoxd_float2(Position2);
    zoxd_float(Rotation2);
    zoxd_float2(Scale2);
    zoxd_float2(LocalPosition2);
    zoxd_float(LocalRotation2);
    zoxd_float2(LocalScale2);
    zoxd(LinePoints2);
    zoxd(LocalLinePoints2);
}
