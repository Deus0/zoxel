#define bounce_multiplier 1.0f
float bounce_threshold = 0.001f;
float3 bounce_lost_force = (float3) { 0.32f, 0, 0.32f }; // 0.32 | 0.12
float bounce_dampening = 0.48f;
// #define bounce_dampening 0.48f
// collision
const float collision_precision = 1.0f;
const float grounded_velocity_threshold = 0.22f;
const float lowest_velocity_threshold = 0.03f; // 0.3 // 0.001
// draw line to voxel side from character...
// debugs
// #define zox_gizmos_sphere_colliders
byte is_debug_collisions = 0;
