// =======================================
// print_octree_recursive.h
// Generic octree printer for ptr-first structs
// =======================================

static void print_octree_recursive(const void* node, byte depth, byte target_depth, byte3 pos, size_t stride, size_t value_offset, const char* prefix) {
    if (!node) return;

    // At target depth, print value
    if (depth == target_depth) {
        byte value = *(const byte*)((const char*)node + value_offset);
        zox_log("%s[%u,%u,%u] = %u", prefix, pos.x, pos.y, pos.z, value);
        return;
    }

    const void* kids = *(const void* const*)node;
    if (!kids) return; // stop if children missing

    // Recurse into 8 children
    for (byte i = 0; i < 8; i++) {
        byte3 child_pos = { pos.x*2 + (i&1), pos.y*2 + ((i>>1)&1), pos.z*2 + ((i>>2)&1) };
        print_octree_recursive((const char*)kids + i*stride, depth+1, target_depth, child_pos, stride, value_offset, prefix);
    }
}

// Macro wrapper to keep old interface
#define print_octree_layer(root, target_depth, TypeName) \
do { \
    byte3 pos = {0,0,0}; \
    print_octree_recursive((const void*)(root), 0, (target_depth), pos, sizeof(TypeName), offsetof(TypeName, value), #TypeName); \
} while(0)
