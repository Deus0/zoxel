/* EcsOnLoad
 * EcsPostLoad
 * EcsPreUpdate
 * EcsOnUpdate
 * EcsOnValidate
 * EcsPostUpdate
 * EcsPreStore
 * EcsOnStore
 */

// 1) Spawn
// 2) Main  - Multithreaded
// 3) GPU Uploads + Rendering

#define zoxp_dbg_begin EcsOnLoad
#define zoxp_dbg_end EcsOnStore

// Events
// NOTE: Remove before spawn so it lasts a frame
#define zoxp_remove EcsPostLoad       // Remove components, events
// regular sync points
// #define zoxp_start EcsPreUpdate
#define zoxp_spawn EcsPreUpdate          // EcsPreStore // EcsPostUpdate
#define zoxp_update EcsOnUpdate
#define zoxp_post_update EcsOnValidate
#define zoxp_gpu_upload EcsPreStore     // zoxp_cameras +
#define zoxp_rendering EcsPreStore      // zoxp_cameras +
// Core
#define zoxp_physics EcsOnUpdate
#define zoxp_physics_apply EcsOnValidate
#define zoxp_transforms EcsOnValidate
#define zoxp_cameras EcsPostUpdate
// system based
#define zoxp_state EcsOnUpdate
#define zoxp_reset EcsPostUpdate
#define zoxp_destroy EcsOnUpdate // EcsPostUpdate
// doesnt seem to mind if its in same frame as zoxp_cameras
// Inputs
#define zoxp_inputs_extract zoxp_spawn
#define zoxp_inputs_reset zoxp_reset
#define zoxp_inputs_enable zoxp_update
#define zoxp_inputs_update zoxp_update
// (VoxelNode) Queue
// TODO: Process Queue for Octree in frame a fter, perhaps a second stack?
#define zoxp_queue_add zoxp_update
#define zoxp_queue_process zoxp_update
#define zoxp_queue_pre_clear EcsOnValidate
#define zoxp_queue_pre_post_clear EcsOnValidate
#define zoxp_queue_clear EcsOnValidate

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
