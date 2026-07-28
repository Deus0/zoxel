#define initial_dynamic_array_size 16

// used in many functions - todo: rename to arraye (expandable)
#define zoxel_dynamic_array(data_type)\
typedef struct {\
    data_type *data;\
    size_t capacity;\
    size_t size;\
} data_type##_array_d;\
\
data_type##_##array_d* create_##data_type##_array_d(size_t initial_capacity) {\
    data_type##_##array_d* dynamic_array = malloc(sizeof(data_type##_##array_d));\
    dynamic_array->data = malloc(initial_capacity * sizeof(data_type));\
    dynamic_array->capacity = initial_capacity;\
    dynamic_array->size = 0;\
    return dynamic_array;\
}\
\
void dispose_##data_type##_array_d(data_type##_array_d* dynamic_array) {\
    free(dynamic_array->data);\
    free(dynamic_array);\
}\
\
void data_type##_array_d_add(data_type##_array_d* dynamic_array, data_type array_entry) {\
    if (dynamic_array->size == dynamic_array->capacity) {\
        dynamic_array->capacity *= 2;\
        dynamic_array->data = realloc(dynamic_array->data, dynamic_array->capacity * sizeof(data_type));\
    }\
    if (dynamic_array->data) {\
        dynamic_array->data[dynamic_array->size++] = array_entry;\
    } else { \
        zox_loge("Realloc Failed in [array_d_add]"); \
    } \
}\
\
void expand_capacity_##data_type##_array_d(data_type##_##array_d* dynamic_array, int add_count) {\
    size_t required_capacity = dynamic_array->size + add_count;\
    if (required_capacity > dynamic_array->capacity) {\
        dynamic_array->capacity *= 2;\
        dynamic_array->data = realloc(dynamic_array->data, dynamic_array->capacity * sizeof(data_type));\
    }\
}\
\
void add_block_to_##data_type##_array_d(data_type##_array_d* dynamic_array, const data_type block[], byte length) {\
    size_t required_capacity = dynamic_array->size + length; \
    if (required_capacity > dynamic_array->capacity) { \
        dynamic_array->capacity *= 2;\
        dynamic_array->data = realloc(dynamic_array->data, dynamic_array->capacity * sizeof(data_type));\
    }\
    memcpy(dynamic_array->data + dynamic_array->size, block, length * sizeof(data_type)); \
    dynamic_array->size += length; \
}\
\
void add_block_to##_##data_type##_##array_d2(data_type##_##array_d* dynamic_array, const data_type block[], byte length) { \
    size_t required_capacity = dynamic_array->size + length;\
    if (required_capacity > dynamic_array->capacity) {\
        dynamic_array->capacity *= 2;\
        dynamic_array->data = realloc(dynamic_array->data, dynamic_array->capacity * sizeof(data_type));\
    }\
    memcpy(dynamic_array->data + dynamic_array->size, block, length * sizeof(data_type));\
    for (byte i = 0; i < length; i++) dynamic_array->data[dynamic_array->size + i] = block[i];\
    dynamic_array->size += length; \
}\
\
data_type* finalize_##data_type##_##array_d(data_type##_##array_d* dynamic_array) {\
    if (!dynamic_array->size) {\
        dispose##_##data_type##_##array_d(dynamic_array);\
        return NULL;\
    } else if (dynamic_array->size == dynamic_array->capacity) {\
        data_type* data = dynamic_array->data;\
        free(dynamic_array);\
        return data;\
    } else {\
        data_type* data = realloc(dynamic_array->data, dynamic_array->size * sizeof(data_type));\
        free(dynamic_array);\
        return data;\
    }\
}\
\
data_type* finalize_arrayd_##data_type(data_type##_array_d* dynamic_array) {\
    if (!dynamic_array->size) {\
        dispose##_##data_type##_##array_d(dynamic_array);\
        return NULL;\
    } else {\
        data_type* data = malloc(dynamic_array->size * sizeof(data_type)); \
        memcpy(data, dynamic_array->data, dynamic_array->size * sizeof(data_type)); \
        dispose##_##data_type##_array_d(dynamic_array);\
        return data;\
    }\
}

#define create_array_d_has(data_type)\
\
byte data_type##_array_d_has(data_type##_array_d* dynamic_array, data_type value) {\
    for (size_t i = 0; i < dynamic_array->size; i++) {\
        if (dynamic_array->data[i] == value) {\
            return 1;\
        }\
    }\
    return 0;\
} \
\
int data_type##_array_d_index(data_type##_##array_d* dynamic_array, data_type value) {\
    for (size_t i = 0; i < dynamic_array->size; i++) {\
        if (dynamic_array->data[i] == value) {\
            return i;\
        }\
    }\
    return -1;\
}
