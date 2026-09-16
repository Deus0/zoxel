// Dynamic Arrays
#define zoxc_arrayd(T, type) \
\
typedef struct { \
    int length; \
    type* value; \
    spinlock lock; \
} T; \
\
zoxc_custom(T); \
\
ECS_CTOR(T, ptr, { \
    ptr->length = 0; \
    ptr->value = NULL; \
    spinlock_init(&ptr->lock); \
}) \
\
void dispose_##T(T* ptr) { \
    spin_lock(&ptr->lock); \
    if (ptr->value) { \
        zox_free(ptr->value); \
        ptr->value = NULL; \
    }\
    ptr->length = 0;\
    spin_unlock(&ptr->lock); \
}\
\
ECS_DTOR(T, ptr, { \
    dispose_##T(ptr); \
})\
\
ECS_MOVE(T, dst, src, { \
    dst->value = src->value; \
    dst->length = src->length; \
    spinlock_init(&dst->lock); \
    src->value = NULL; \
    src->length = 0; \
}) \
\
void clone_##T(T* dst, const T* src) {\
    if (dst == src) {\
        return;\
    }\
    spin_lock(&dst->lock); \
    spin_lock((spinlock*)&src->lock); \
    if (dst->value) { \
        zox_free(dst->value); \
        dst->value = NULL; \
        dst->length = 0;\
    }\
    if (src->value) {\
        int memory_length = src->length * sizeof(type);\
        type* ptr = zox_malloc(memory_length);\
        if (!ptr) {\
            zox_loge("malloc failed clone_" #T);\
            spin_unlock((spinlock*)&src->lock); \
            spin_unlock(&dst->lock); \
            return;\
        }\
        memcpy(ptr, src->value, memory_length);\
        dst->value = ptr;\
        dst->length = src->length;\
    }\
    spin_unlock((spinlock*)&src->lock); \
    spin_unlock(&dst->lock); \
}\
\
ECS_COPY(T, dst, src, { \
    clone_##T(dst, src); \
}) \
\
void initialize_##T(T* ptr, int length) {\
    spin_lock(&ptr->lock); \
    if (length > 0) {\
        type *new_memory = zox_malloc(length * sizeof(type));\
        if (!new_memory) {\
            zox_loge("malloc failure " #T);\
        } else {\
            ptr->value = new_memory;\
            ptr->length = length;\
        }\
    } \
    spin_unlock(&ptr->lock); \
} \
\
void resize_##T(T* ptr, int length) {\
    spin_lock(&ptr->lock); \
    if (ptr->length != length) {\
        if (!length) {\
            if (ptr->value) { \
                zox_free(ptr->value); \
                ptr->value = NULL; \
            } \
            ptr->length = 0; \
        } else if (!ptr->value) {\
            type* new_memory = zox_malloc(length * sizeof(type)); \
            if (!new_memory) { \
                zox_loge("malloc failure " #T); \
            } else { \
                ptr->value = new_memory; \
                ptr->length = length; \
            } \
        } else {\
            type* new_memory = zox_realloc(ptr->value, length * sizeof(type));\
            if (!new_memory) {\
                zox_loge("Failure with realloc");\
            } else {\
                ptr->value = new_memory;\
                ptr->length = length;\
            }\
        }\
    }\
    spin_unlock(&ptr->lock); \
} \
\
byte add_to_##T(T *ptr, type data) { \
    spin_lock(&ptr->lock); \
    int new_length = ptr->length + 1; \
    type* new_value = ptr->value \
        ? zox_realloc(ptr->value, new_length * sizeof(type)) \
        : zox_malloc(new_length * sizeof(type)); \
    if (!new_value) { \
        zox_loge("malloc failed in add_to_" #T); \
        spin_unlock(&ptr->lock); \
        return 0; \
    } \
    ptr->value = new_value; \
    ptr->value[ptr->length] = data; \
    ptr->length++; \
    spin_unlock(&ptr->lock); \
    return 1; \
} \
\
byte remove_at_##T(T *ptr, int index) {\
    spin_lock(&ptr->lock); \
    if (!ptr->value || index < 0 || index >= ptr->length) {\
        spin_unlock(&ptr->lock); \
        return 0;\
    }\
    for (int i = index; i < ptr->length - 1; i++) {\
        ptr->value[i] = ptr->value[i + 1];\
    }\
    ptr->length--;\
    if (!ptr->length) {\
        zox_free(ptr->value);\
        ptr->value = NULL;\
    } else {\
        ptr->value = zox_realloc(ptr->value, ptr->length * sizeof(type));\
    }\
    spin_unlock(&ptr->lock); \
    return 1;\
}

#define zoxd_arrayd(T)\
    zoxd(T);\
    zox_define_hooks(T);
