// the sun lighting lit meshes (colored3D_lit): light = ambient + sun * the face's facing to it; a game may set its own
float3 render_sun_direction = { -0.5f, 1, -0.3f };     // towards the sun, normalized when drawn
float3 render_sun_light = { 0.65f, 0.65f, 0.65f };
float3 render_ambient_light = { 0.35f, 0.35f, 0.35f };
