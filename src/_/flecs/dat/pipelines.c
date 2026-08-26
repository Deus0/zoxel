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
#define zoxp_start EcsPreUpdate
#define zoxp_update EcsOnUpdate
#define zoxp_post_update EcsOnValidate
#define zoxp_spawn EcsPostUpdate          // EcsPreStore // EcsPostUpdate
#define zoxp_gpu_upload EcsPreStore     // zoxp_cameras +
#define zoxp_rendering EcsPreStore      // zoxp_cameras +
#define zoxp_end EcsOnStore
#define zoxp_mainthread EcsPreStore     // EcsPreStore // EcsPostUpdate
// Core
#define zoxp_physics EcsOnUpdate
#define zoxp_physics_apply EcsOnValidate
#define zoxp_transforms EcsOnValidate
#define zoxp_cameras EcsPostUpdate
// system based
#define zoxp_state zoxp_start
#define zoxp_reset EcsPostUpdate
#define zoxp_destroy EcsPostUpdate
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
#define zoxp_queue_pre_clear zoxp_queue_process + 1
#define zoxp_queue_pre_post_clear zoxp_queue_process + 1
#define zoxp_queue_clear zoxp_queue_pre_clear + 2

// Voxels
/*#define zoxp_voxels_generate EcsOnUpdate    // Write to voxels
#define zoxp_voxels_sides EcsOnValidate      // Write to mesh from voxels
#define zoxp_voxels_mesh EcsPostUpdate      // Write to mesh from voxels
#define zoxp_voxels_lights EcsPostUpdate      // Write to mesh from voxels
*/

// Voxels
#define zoxp_voxels_generate zoxp_update    // Write to voxels
#define zoxp_voxels_sides zoxp_update      // Write to mesh from voxels
#define zoxp_voxels_mesh zoxp_update      // Write to mesh from voxels
#define zoxp_voxels_lights zoxp_update      // Write to mesh from voxels

#define zoxp_dbg_begin EcsOnLoad
#define zoxp_dbg_end EcsOnStore
