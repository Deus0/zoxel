#define zoxc_octree_fun2(T, base)\
\
void clone_at_depth_##T(T* dst, const T* src, byte target_depth, byte depth) {\
    if (target_depth > 0 && depth == target_depth - 1) {\
        if (src->ptr) {\
            open_##T(dst);\
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
const T* get_adjacent_##T(\
    const T** neighbors,\
    const T* root,\
    int3 position,\
    byte depth,\
    byte dir,\
    byte *chunk_index \
) {\
    if (depth >= 8) {\
        return NULL;\
    }\
    position = move_position(position, dir); \
    byte b = powers_of_two[depth];\
    if (position.x >= 0 && position.x < b && \
        position.y >= 0 && position.y < b && \
        position.z >= 0 && position.z < b) { \
        return get_##T(root, depth, int3_to_byte3(position), 0); \
        /*return gett_##T(node, position, depth);*/\
    } else {\
        /* special case for adjacent ptr, flips position and crosses to neighbor chunk */\
        *chunk_index = dir + 1;\
        const T* n = neighbors[dir]; \
        position = reverse_position(position, dir, b); \
        return get_##T(n, depth, int3_to_byte3(position), 0); \
        /*return gett_##T(n, position, depth); */\
    }\
}\
\
\
const T* get_adjacentn_##T(\
    const T** neighbors, \
    const T* vnode,\
    int3 position, \
    byte depth, \
    byte direction \
) {\
    if (!vnode) { \
        return NULL; \
    } \
    \
    position = move_position(position, direction); \
    byte vlength = powers_of_two[depth];\
    \
    if (position.x < 0 || position.x >= vlength || \
        position.y < 0 || position.y >= vlength || \
        position.z < 0 || position.z >= vlength) { \
        \
        position = reverse_position(position, direction, vlength); \
        vnode = neighbors[direction];\
        \
        if (!vnode) { \
            return NULL; \
        } \
    }\
    \
    const T* v = get_##T(vnode, depth, int3_to_byte3(position), 0);\
    return v; \
}

byte is_on_edge_octree(byte depth, int3 position, byte direction) {
    if (depth >= 8) {
        return 0;
    }
    position = move_position(position, direction);
    return !(position.x >= 0 && position.x < powers_of_two[depth] && position.y >= 0 && position.y < powers_of_two[depth] &&  position.z >= 0 && position.z < powers_of_two[depth]);
}

byte get_adjacent_depth(byte depth,const byte* ndepths, int3 position, byte direction) {
    if (is_on_edge_octree(depth, position, direction)) {
        return ndepths[direction];
    } else {
        return depth;
    }
}

/* if depth finish or if closed node, return node early */
/*
 base find_node_value_##T(\
 const T* node,\
 int3 position,\
 byte depth)\
 if (depth == 0 || node->ptr == NULL) {\
 {\
     return node->value;\
 }\
 depth--;\
 byte dividor = powers_of_two[depth];\
 int3 local_position = (int3) {\
     position.x / dividor,\
     position.y / dividor,\
     position.z / dividor\
 };\
 int3 child_octree_position = (int3) {\
     position.x % dividor,\
     position.y % dividor,\
     position.z % dividor\
 };\
 T* kids = get_children_##T(node);\
 return find_node_value_##T(&kids[int3_to_node_index(local_position)], child_octree_position, depth);\
 }\
\
const T* gett_##T(\
    const T* node,\
    int3 position,\
    byte depth \
) {\
    if (!node || depth >= 8) { \
        zox_log_error("invalid node or depth: in get_# T"); \
        return NULL; \
    } \
    if (!depth || !has_children_##T(node)) {\
        return node;\
    }\
    depth--;\
    byte dividor = powers_of_two[depth];\
    int3 node_position = (int3) { \
        position.x / dividor,\
        position.y / dividor,\
        position.z / dividor\
    }; \
    int i = int3_to_node_index(node_position); \
    int3 child_position = (int3) {\
        position.x % dividor,\
        position.y % dividor,\
        position.z % dividor\
    };\
    T* kids = get_children_##T(node); \
    return gett_##T( \
        &kids[i], \
        child_position, \
        depth); \
}\


\
void clone_depth_##T(\
    T* dst,\
    const T* src,\
    const byte max_depth,\
    byte depth \
) {\
    dst->value = src->value;\
    dst->type = src->type;\
    depth++;\
    if (src->ptr && depth <= max_depth) {\
        open_##T(dst);\
        T* kids_dst = get_children_##T(dst);\
        T* kids_src = get_children_##T(src);\
        for (byte i = 0; i < octree_length; i++) {\
            clone_depth_##T(&kids_dst[i], &kids_src[i], max_depth, depth);\
        }\
    } else {\
        dst->ptr = src->ptr;\
    }\
} \
 */
