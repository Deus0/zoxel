#define zoxc_octree_fun2(T, base)\
\
void clone_at_depth_##T(T* dst, const T* src, byte target_depth, byte depth) {\
    if (target_depth > 0 && depth == target_depth - 1) {\
        if (src->ptr) {\
            open_one_##T(dst);\
        }\
    }\
    \
    if (depth == target_depth) {\
        dst->value = src->value;\
        /*dst->ptr = src->ptr;*/\
        /*dst->type = src->type;*/\
    } else {\
        if (src->ptr && dst->ptr) {\
            depth++;\
            T* kids_src = get_children_##T(src);\
            T* kids_dst = get_children_##T(dst);\
            for (byte i = 0; i < octree_length; i++) {\
                clone_at_depth_##T(&kids_dst[i], &kids_src[i], target_depth, depth);\
            }\
        }\
    }\
}\
\
\
/* maybe make below function use this if it isn't in the non root node */\
const T* get_adjacent_##T(const T** neighbors, const T* root, int3 position, byte depth, byte dir, byte *chunk_index) {\
    if (depth >= 8) {\
        return NULL;\
    }\
    position = move_position(position, dir); \
    short length = octree_size(depth); \
    if (position.x >= 0 && position.x < length && \
        position.y >= 0 && position.y < length && \
        position.z >= 0 && position.z < length) { \
        return get_##T(root, depth, int3_to_byte3(position)); \
        /*return gett_##T(node, position, depth);*/\
    } else {\
        /* special case for adjacent ptr, flips position and crosses to neighbor chunk */\
        *chunk_index = dir + 1;\
        const T* n = neighbors[dir]; \
        position = reverse_position(position, dir, length); \
        return get_##T(n, depth, int3_to_byte3(position)); \
        /*return gett_##T(n, position, depth); */\
    }\
}\
\
\
const T* get_adjacentn_##T(const T** neighbors, const T* vnode, int3 position, byte depth, byte direction) {\
    if (!vnode) { \
        return NULL; \
    } \
    short length = octree_size(depth); \
    position = move_position(position, direction); \
    if (position.x < 0 || position.x >= length || \
        position.y < 0 || position.y >= length || \
        position.z < 0 || position.z >= length) { \
        position = reverse_position(position, direction, length); \
        vnode = neighbors[direction];\
        if (!vnode) { \
            return NULL; \
        } \
    }\
    \
    return get_##T(vnode, depth, int3_to_byte3(position));\
}

byte is_on_edge_octree(byte depth, int3 position, byte direction) {
    if (depth >= 8) {
        return 0;
    }
    short length = octree_size(depth);
    position = move_position(position, direction);
    return !(position.x >= 0 && position.x < length && position.y >= 0 && position.y < length &&  position.z >= 0 && position.z < length);
}

byte get_adjacent_depth(byte depth,const byte* ndepths, int3 position, byte direction) {
    if (is_on_edge_octree(depth, position, direction)) {
        return ndepths[direction];
    } else {
        return depth;
    }
}