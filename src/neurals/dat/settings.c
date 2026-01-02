// #define zox_test_brain
// #define zox_render_brain
// #define zox_log_neurals
const float neural_process_speed = 32.0f;
const int brain_layers = 16;
const int2 brain_texture_size = { 16, 16 };
const float neural_render_scale = 16.0f;
const float neural_render_scale2 = 1.0f; // thickneess
const double neural_brain_delay = 0;
const float2 neural_position = { 0, 16 };
entity local_brain = 0;
entity brain_texture = 0;
entity brain_texture_ui = 0;
