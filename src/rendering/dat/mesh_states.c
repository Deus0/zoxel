#define mesh_state_none 0
// trigger_slow -> generate
#define mesh_state_trigger_terrain 1   // For clearing meshes
#define mesh_state_trigger_slow_end 5   // For clearing meshes
// normal flow
#define mesh_state_skeleton_trigger 6    // for non skeletons, we can trigger faster
#define mesh_state_skeleton_generate 7
// upload to gpu
#define mesh_state_trigger 7    // for non skeletons, we can trigger faster
#define mesh_state_upload 8
#define mesh_state_end 9
