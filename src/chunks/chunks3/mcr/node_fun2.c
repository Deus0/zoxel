int3 reverse_position(int3 position, byte d, byte length) {
    if (d == direction_left) {
        position.x = length - 1;
    } else if (d == direction_right) {
        position.x = 0;
    } else if (d == direction_down) {
        position.y = length - 1;
    } else if (d == direction_up) {
        position.y = 0;
    } else if (d == direction_back) {
        position.z = length - 1;
    } else if (d == direction_front) {
        position.z = 0;
    }
    return position;
}

int3 move_position(int3 position, byte dir) {
    if (dir == direction_left) {
        position.x--;
    } else if (dir == direction_right) {
        position.x++;
    } else if (dir == direction_down) {
        position.y--;
    } else if (dir == direction_up) {
        position.y++;
    } else if (dir == direction_back) {
        position.z--;
    } else if (dir == direction_front) {
        position.z++;
    }
    return position;
}

#define zoxc_octree_fun2(T, base)\
\
base find_node_value_##T(\
    const T* node,\
    int3 position,\
    byte depth)\
{\
    /* if depth finish or if closed node, return node early */ \
    if (depth == 0 || node->ptr == NULL) {\
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
void clone_at_depth_##T(\
    T* dst,\
    const T* src,\
    const byte target_depth,\
    byte depth \
) {\
    if (target_depth > 0 && depth == target_depth - 1) {\
        if (src->ptr) {\
            open_##T(dst);\
        }\
    }\
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
        /*dst->ptr = NULL;*/\
    }\
} \
\
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
/* maybe make below function use this if it isn't in the non root node */\
const T* get_adjacent_##T(\
    const T** neighbors,\
    const T* node,\
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
        return gett_##T(node, position, depth);\
    } else {\
        /* special case for adjacent ptr, flips position and crosses to neighbor chunk */\
        *chunk_index = dir + 1;\
        const T* n = neighbors[dir]; \
        position = reverse_position(position, dir, b); \
        return gett_##T(n, position, depth); \
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
}\
\
byte get_adjacent_depth_##T(\
    const T** neighbors, \
    const byte* ndepths, \
    int3 position,\
    byte depth,\
    byte dir\
) {\
    if (depth >= 8) { \
        return depth; \
    } \
    position = move_position(position, dir); \
    byte b = powers_of_two[depth]; \
    if (position.x >= 0 && position.x < b && position.y >= 0 && position.y < b && position.z >= 0 && position.z < b) { \
        return depth; \
    } else { \
        if (neighbors[dir]) { \
            return ndepths[dir]; \
        } else { \
            return depth; \
        } \
    } \
}


    /*return gett_##T( \
        voctree, \
        position, \
        depth \
    );\*/

/*const T* get_root_adjacent_##T(\
    const T** neighbors,\
    const T* root,\
    const T* node,\
    int3 position,\
    byte node_index,\
    byte3 node_position,\
    byte depth,\
    byte direction,\
    byte *chunk_index \
) {\
    if (has_children_##T(node)) {\
        T* kids = get_children_##T(node);\
        if (direction == direction_left) {\
            if (node_position.x != 0) {\
                return &kids[node_index_with_left[node_index]];\
            }\
        } else if (direction == direction_right) {\
            if (node_position.x != 1) {\
                return &kids[node_index_with_right[node_index]];\
            }\
        } else if (direction == direction_down) {\
            if (node_position.y != 0) {\
                return &kids[node_index_with_down[node_index]];\
            }\
        } else if (direction == direction_up) {\
            if (node_position.y != 1) {\
                return &kids[node_index_with_up[node_index]];\
            }\
        } else if (direction == direction_back) {\
            if (node_position.z != 0) {\
                return &kids[node_index_with_back[node_index]];\
            }\
        } else if (direction == direction_front) {\
            if (node_position.z != 1) {\
                return &kids[node_index_with_front[node_index]];\
            }\
        }\
    }\
    if (root != NULL) {\
        return get_adjacent_##T( \
            neighbors, \
            root, \
            position, \
            depth, \
            direction, \
            chunk_index);\
    }\
    return NULL;\
}\*/

