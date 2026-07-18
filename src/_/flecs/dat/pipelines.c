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
#define zoxp_reset EcsOnValidate
#define zoxp_destroy EcsOnValidate
#define zoxp_mainthread EcsPreStore
#define zoxp_rendering EcsOnStore // zoxp_cameras + 1
// doesnt seem to mind if its in same frame as zoxp_cameras
#define zoxp_physics zoxp_update // EcsPreUpdate           // EcsPostUpdate
#define zoxp_transforms EcsOnValidate // zoxp_physics + 1
#define zoxp_cameras EcsPostUpdate // zoxp_transforms + 1    // CameraPlanes/Matrix
// Inputs
#define zoxp_inputs_reset zoxp_reset // EcsOnLoad
#define zoxp_inputs_update zoxp_update
#define zoxp_inputs_extract zoxp_mainthread //  EcsPostLoad
#define zoxp_inputs_enable zoxp_update // EcsPostLoad
// (VoxelNode) Queue
// TODO: Process Queue for Octree in frame a fter, perhaps a second stack?
#define zoxp_queue_add EcsOnLoad // EcsPostLoad
#define zoxp_queue_process zoxp_update
#define zoxp_queue_clear zoxp_reset
#define zoxp_queue_pre_clear zoxp_queue_clear - 1
