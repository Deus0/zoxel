#include "lods.c"
#include "bounds.c"
#include "voctree.c"
#include "positioning.c"
#include "ground.c"
#include "unlit.c"
#include "ao6.c"
#include "ao27.c"

// NOTE: Assumes node depth is lower than terrain (max depth)
static inline float get_chunk_scale(
    byte chunk_depth,
    byte terrain_depth,
    float terrain_scale)
{
    if (terrain_depth < chunk_depth) {
        return terrain_scale;
    } else {
        // we multiply by the depth difference power
        // - if 2 = 2*2 = 4, 0.5 becomes 2 in size
        byte ddepth = terrain_depth - chunk_depth;
        return terrain_scale * octree_size(ddepth);
    }
}

static inline void initialize_voxel_lock(
    ecs* world,
    entity e)
{
    spinlock vlock;
    spinlock_init(&vlock);
    zox_setv(e, VoxelNodeLock, vlock);
}

static inline void initialize_sides_lock(
    ecs* world,
    entity e)
{
    spinlock lock;
    spinlock_init(&lock);
    zox_setv(e, SidesOctreeLock, lock);
}



entity spawn_texture_from_vox(
    ecs* world,
    entity vox,
    byte2 size,
    byte face)
{
    // # # # Spawn Item Texture # # #
    entity texture = spawn_texture(
        world,
        prefab_vox_texture,
        byte2_to_int2(size));
    {
        char name2[128];
        sprintf(name2, "texture_%s", zox_getn(vox));
        zox_set_unique_name(texture, name2);
    }
    // zox_set_name_e(texture, "bodys_texture_head");
    zox_setv(texture, VoxBakeSide, face);
    zox_setv(texture, ModelLink, vox);
    delay_event(
        world,
        &delayed_texture_generate,
        texture,
        1.0f);
    return texture;
}
