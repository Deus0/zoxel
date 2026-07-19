/* EcsOnLoad
 * EcsPostLoad
 * EcsPreUpdate
 * EcsOnUpdate
 * EcsOnValidate
 * EcsPostUpdate
 * EcsPreStore
 * EcsOnStore
 */
// regular sync points
#define zoxp_start EcsPostLoad
#define zoxp_update EcsOnUpdate
#define zoxp_post_update EcsOnValidate
#define zoxp_mainthread EcsPreStore // EcsPostUpdate
#define zoxp_rendering EcsPreStore // zoxp_cameras +
#define zoxp_end EcsOnStore
// Core
#define zoxp_physics EcsPreUpdate
#define zoxp_transforms zoxp_physics + 1
#define zoxp_cameras zoxp_transforms + 1
// system based
#define zoxp_state zoxp_start
#define zoxp_reset zoxp_post_update
#define zoxp_destroy zoxp_post_update
// doesnt seem to mind if its in same frame as zoxp_cameras
// Inputs
#define zoxp_inputs_reset zoxp_reset
#define zoxp_inputs_enable zoxp_update
#define zoxp_inputs_update zoxp_update
#define zoxp_inputs_extract zoxp_mainthread
// (VoxelNode) Queue
// TODO: Process Queue for Octree in frame a fter, perhaps a second stack?
#define zoxp_queue_add zoxp_start
#define zoxp_queue_process zoxp_update
#define zoxp_queue_clear zoxp_reset
#define zoxp_queue_pre_clear zoxp_queue_clear - 1
