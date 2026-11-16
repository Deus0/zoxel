// Free roam: a camera flown by two sticks (xr/tst/cube.c's, for the engine's XR camera, and any platform)
//  - the right stick turns: x yaws, y pitches
//  - the left stick moves where it looks: its rotation, then the head's (a headset's, identity without one),
//      forward is -z (OpenXR's)
//  - per second, so it's the same at any frame rate (tst camera_free_roam)
#define free_roam_move_speed 3.0f      // metres a second, a stick held full
#define free_roam_turn_speed 2.0f      // radians a second

typedef struct {
    float3 position;
    float3 euler;
} free_roam;

static inline float4 free_roam_rotation(const free_roam* roam) {
    return quaternion_from_euler(roam->euler);
}

static inline void free_roam_step(free_roam* roam, float2 move, float2 turn, float4 head_rotation, float delta) {
    roam->euler.x += turn.y * free_roam_turn_speed * delta;
    roam->euler.y -= turn.x * free_roam_turn_speed * delta;
    const float4 aim = quaternion_rotate(free_roam_rotation(roam), head_rotation);
    const float3 forward = float4_rotate_float3(aim, (float3) { 0, 0, -1 });
    const float3 right = float4_rotate_float3(aim, (float3) { 1, 0, 0 });
    const float speed = free_roam_move_speed * delta;
    roam->position = float3_add(roam->position, float3_add(float3_scale(right, move.x * speed), float3_scale(forward, move.y * speed)));
}
