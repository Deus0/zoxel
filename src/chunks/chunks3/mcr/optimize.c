// =======================================
// zox_node_optimizer.h
// Octree Optimizer (sets parent value to most common child value)
// =======================================

static inline void optimize_octree_node(
    void* node,
    size_t stride,
    size_t value_offset
) {
    if (!node) return;

    void** ptr = (void**)node;
    void* kids = *ptr;

    // Leaf node, nothing to optimize
    if (!kids) return;

    // Recurse bottom-up
    for (byte i = 0; i < 8; i++) {
        optimize_octree_node((char*)kids + i * stride, stride, value_offset);
    }

    // Count child values (max 8)
    byte child_vals[8];
    byte child_count = 0;
    for (byte i = 0; i < 8; i++) {
        void* child = (char*)kids + i * stride;
        byte val = *(byte*)((char*)child + value_offset);

        // Track unique values and counts
        byte j;
        for (j = 0; j < child_count; j++) {
            if (child_vals[j] == val) break;
        }
        if (j == child_count) child_vals[child_count++] = val;
    }

    // Simple frequency array for mode (up to 8 values)
    int counts[8] = {0};
    for (byte i = 0; i < 8; i++) {
        void* child = (char*)kids + i * stride;
        byte val = *(byte*)((char*)child + value_offset);
        for (byte j = 0; j < child_count; j++) {
            if (child_vals[j] == val) {
                counts[j]++;
                break;
            }
        }
    }

    // Find mode
    int max_count = -1;
    byte mode_val = 0;
    for (byte i = 0; i < child_count; i++) {
        if (counts[i] > max_count) {
            max_count = counts[i];
            mode_val = child_vals[i];
        }
    }

    // Set parent value
    *(byte*)((char*)node + value_offset) = mode_val;
}

// Macro wrapper
#define create_node_optimizer(T) \
static inline void optimize_##T(T* node) { \
    optimize_octree_node((void*)node, sizeof(T), offsetof(T, value)); \
}

// Usage:
// create_node_optimizer(LightNode)
// optimize_LightNode(lroot);
