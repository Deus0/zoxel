// a chest, shaped like a person's (a v from the shoulders): four ellipsoid blobs in its blueprint bounds (nsize)
//  - belly: narrow, at the bottom, up into the ribs
//  - ribs: wider, the upper chest
//  - yoke: the full width, a band where the shoulders join
//  - waist: between the belly and the ribs, so they're one body, not two blobs pinched together
//  - flatter front to back than across, its depth is the realm's scale (gam/zoxel/sys/body_parts.c)
//  - clothes are fitted to the same blobs (chest_blob_box, spawn_blueprint_shirt)
#define chest_blobs_count 4
typedef struct {
    byte3 center;
    byte3 radius;   // a fill covers center ± radius (voctree_fill_ellipsoid), its size node is twice this
} chest_blob;

static inline void get_chest_blobs(byte3 nsize, chest_blob* blobs) {
    const byte top = 2 * (nsize.y / 2) - 1;     // the highest voxel, under the bounds' top
    const byte cx = nsize.x / 2, cz = nsize.z / 2;
    // the belly reaches up into the ribs, so they overlap where both are wide (top / 4 met only their tips: a pinch,
    //  tst model_shapes)
    const byte3 belly = { (3 * nsize.x) / 10, (5 * top) / 16, (2 * nsize.z) / 5 };
    const byte3 ribs = { (2 * nsize.x) / 5, (5 * top) / 16, nsize.z / 2 };
    const byte3 yoke = { nsize.x / 2, top / 8, (2 * nsize.z) / 5 };
    blobs[0] = (chest_blob) { { cx, belly.y, cz }, belly };
    blobs[1] = (chest_blob) { { cx, top - ribs.y, cz }, ribs };
    blobs[2] = (chest_blob) { { cx, top - yoke.y - 1, cz }, yoke };
    // the waist joins them, between the belly's widest and the ribs' (last, the shirt reads 0 to 2 by place)
    const byte belly_middle = belly.y, ribs_middle = top - ribs.y;
    const byte3 waist = { (7 * nsize.x) / 20, (ribs_middle - belly_middle) / 2 + 1, (9 * nsize.z) / 20 };
    blobs[3] = (chest_blob) { { cx, (belly_middle + ribs_middle) / 2, cz }, waist };
}

// how far the shoulders sit above centred on the chest, in body voxels (character_body_spawn_system): level with the yoke
static inline int body_shoulder_offset_y() {
    const short chest_width = octree_size(block_depth);
    return (5 * int_floorf(0.56f * chest_width)) / 8;
}

// how far the shoulders dig into the chest's sides, in body voxels: 2 at Block Res 4, scaled with it so a finer body
//  keeps its shape (the shoulders' tips under the shirt's sleeves)
static inline byte body_shoulder_dig() {
    return block_depth > 4 ? 2 << (block_depth - 4) : 2;
}

// the box a blob fills: its corner and size, in voxels
static inline void chest_blob_box(chest_blob blob, byte3* corner, byte3* size) {
    *corner = (byte3) { blob.center.x - blob.radius.x, blob.center.y - blob.radius.y, blob.center.z - blob.radius.z };
    *size = (byte3) { 2 * blob.radius.x + 1, 2 * blob.radius.y + 1, 2 * blob.radius.z + 1 };
}

entity spawn_blueprint_chest(
    ecs* world,
    byte3 nsize,
    byte depth,
    color chest_color)
{
    chest_blob blobs[chest_blobs_count];
    get_chest_blobs(nsize, blobs);
    entity e = spawn_node_model_colors(world, chest_color, 1);
    entity previous = e;
    for (byte i = 0; i < chest_blobs_count; i++) {
        const byte3 size = byte3_scale(blobs[i].radius, 2);
        const entity node = spawn_node_model_at(world, prefab_node_model, zox_model_node_fill, blobs[i].center, size, 1);
        zox_set(node, NodeDepth, { depth });
        new_link_single_node(world, previous, node);
        previous = node;
    }
    entity end_node = spawn_node_model_end(world);
    new_link_single_node(world, previous, end_node);
    return e;
}
