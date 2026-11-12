float3 zox_default_gravity = { 0, -9.8, 0 };
const float dissipation_threshold = 0.001f;
const float rotation_friction = 0.56f; // 0.7f;

// angular drag of Frictioned bodies, Omega3D decays as e^(-rate * t) (exactly, see physics/fun)
float omega_dissipation3 = 2.0f;
