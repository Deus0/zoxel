#define mesh_state_none 0
// normal flow
#define mesh_state_skeleton_trigger 1    // for non skeletons, we can trigger faster
#define mesh_state_skeleton_generate 1
#define mesh_state_skeleton_paint 2
#define mesh_state_skeleton_upload 3
#define mesh_state_skeleton_end 3
// upload to gpu
#define mesh_state_trigger 4    // for non skeletons, we can trigger faster
#define mesh_state_upload 4
#define mesh_state_colors_upload 5
#define mesh_state_end 6

// trigger_slow -> generate
// #define mesh_state_trigger_terrain 1   // For clearing meshes
// #define mesh_state_trigger_slow_end 5   // For clearing meshes
