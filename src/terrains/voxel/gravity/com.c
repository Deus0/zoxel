// a node of a chunk's gravity field: a byte a voxel (fun.c), byte only nodes as LightOctree's
typedef struct {
    void* ptr;
    byte value;
} GravityOctree;

zoxc_octree_basic(GravityOctree, byte, 0);
create_octree_setters(GravityOctree);
create_octree_accessors(GravityOctree);
create_octree_reducer(GravityOctree);
zox_tag(GravityDirty);      // its field to work out again (on_gravity_dirty), a non fragmenting event
zox_tag(TerrainGravity);    // a body pulled by the field where it is (terrain_gravity_system), not its own Gravity3D

void zox_components_gravities(ecs* world) {
    zoxd_octree(GravityOctree);
    zoxd_nf_tag(GravityDirty);
    zoxd_tag(TerrainGravity);
}
