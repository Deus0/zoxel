/*EcsOnLoad
 * EcsPostLoad
 * EcsPreUpdate
 * EcsOnUpdate
 * EcsOnValidate
 * EcsPostUpdate
 * EcsPreStore
 * EcsOnStore
 */
#define zoxp_state EcsOnLoad            // EcsOnLoad EcsPreStore
#define zoxp_update EcsOnUpdate         // normal business here
#define zoxp_reset EcsPostUpdate
#define zoxp_state_reset EcsPostUpdate
#define zoxp_destroy EcsPostUpdate
#define zoxp_mainthread EcsPreStore
// doesnt seem to mind if its in same frame as zoxp_cameras
#define zoxp_physics EcsPreUpdate           // EcsPostUpdate
#define zoxp_transforms zoxp_physics + 1    // Transforms EcsPreStore EcsPostUpdate
#define zoxp_cameras zoxp_transforms + 1    // CameraPlanes/Matrix
#define zoxp_rendering EcsOnStore // zoxp_cameras + 1
// Inputs
#define zoxp_inputs_reset EcsOnLoad
#define zoxp_inputs_update zoxp_update
#define zoxp_inputs_extract EcsPostLoad
#define zoxp_inputs_enable EcsPostLoad
// (VoxelNode) Queue
#define zoxp_queue_add EcsPostLoad
#define zoxp_queue_process zoxp_update
#define zoxp_queue_clear EcsPostUpdate // EcsOnLoad // EcsOnStore
#define zoxp_queue_pre_clear zoxp_queue_clear - 1
