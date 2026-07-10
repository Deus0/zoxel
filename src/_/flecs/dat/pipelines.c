//! A bunch of flecs system macros
// todo: make all use of these pipeline tags, zox_ ones because it
//  > makes it easier to organize
// breaks visuals if i put on store

/*#define zox_pip_a EcsOnLoad
#define zox_pip_b EcsPostLoad
#define zox_pip_c EcsPreUpdate
#define zox_pip_d EcsOnUpdate
#define zox_pip_e EcsOnValidate
#define zox_pip_f EcsPostUpdate
#define zox_pip_g EcsPreStore
#define zox_pip_h EcsOnStore*/

#define zoxp_mainthread EcsOnStore      // EcsOnLoad | EcsPreStore | EcsOnStore
#define zoxp_update EcsOnUpdate         // normal business here
#define zoxp_state_reset EcsPreStore    // EcsPostUpdate
#define zoxp_destroy EcsPreStore

#define zoxp_inputs_reset EcsOnLoad
#define zoxp_inputs_extract EcsPostLoad
#define zoxp_inputs_enable EcsPostLoad
#define zoxp_inputs_update zoxp_update

#define zoxp_physics EcsPreUpdate // EcsPostUpdate
#define zoxp_transforms zoxp_physics + 1    // Transforms
#define zoxp_cameras zoxp_transforms + 1    // CameraPlanes/Matrix
// this is EcsOnStore actually
// doesnt seem to mind if its in same frame as zoxp_cameras
#define zoxp_rendering zoxp_cameras + 1

// Data Pipelines
#define zoxp_voxels_write EcsOnUpdate //EcsPostLoad
#define zoxp_lights_write EcsOnUpdate // EcsPreUpdate
#define zoxp_textures zoxp_update           // EcsPostUpdate
#define zoxp_text zoxp_update

// (VoxelNode) Queue
#define zoxp_queue_add EcsPostLoad
#define zoxp_queue_process zoxp_update
#define zoxp_queue_pre_clear EcsPreStore
#define zoxp_queue_clear EcsOnLoad // EcsOnStore

// Rendering Pipelines
// Also breaks if transforms isnt after physics pipeline
// Builds our CameraPlanes, matrix, etc
// builds our camera planes, also camera transforms get updated, needs to be post transforms
// NOTE: Bugs out of we dont update AFTER zoxp_transforms
// #define zoxp_pre_render EcsPreStore         // culls renderers basedon those
