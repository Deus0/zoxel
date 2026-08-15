// Dynamic Arrays
#define zoxc_arrayd(T, type) \
\
typedef struct { \
    int length; \
    type* value; \
    SpinLock lock; \
} T; \
\
zoxc_custom(T); \
\
ECS_CTOR(T, ptr, { \
    ptr->length = 0; \
    ptr->value = NULL; \
    ptr->lock = SPINLOCK_INIT; \
}) \
\
ECS_DTOR(T, ptr, { \
    if (ptr->value) { \
        zox_free(ptr->value); \
        ptr->value = NULL; \
    } \
    ptr->length = 0; \
})\
\
void dispose_##T(T *ptr) { \
    if (ptr->value) { \
        zox_free(ptr->value); \
        ptr->value = NULL; \
    }\
    ptr->length = 0;\
}\
\
ECS_MOVE(T, dst, src, { \
    dst->value = src->value; \
    dst->length = src->length; \
    dst->lock = SPINLOCK_INIT; \
    src->value = NULL; \
    src->length = 0; \
}) \
\
void clone_##T(T* dst, const T* src) {\
    if (dst->value) { \
        zox_free(dst->value); \
        dst->value = NULL; \
        dst->length = 0;\
    }\
    dst->lock = SPINLOCK_INIT; \
    if (src->value) {\
        int memory_length = src->length * sizeof(type);\
        type *value = zox_malloc(memory_length);\
        if (!value) {\
            zox_log_error("malloc failed clone_" #T);\
            return;\
        }\
        memcpy(value, src->value, memory_length);\
        dst->value = value;\
        dst->length = src->length;\
    }\
}\
\
ECS_COPY(T, dst, src, { \
    clone_##T(dst, src); \
}) \
\
void initialize_##T(T* ptr, int length) {\
    if (length > 0) {\
        type *new_memory = zox_malloc(length * sizeof(type));\
        if (!new_memory) {\
            zox_log_error("malloc failure " #T);\
        } else {\
            ptr->value = new_memory;\
            ptr->length = length;\
        }\
    } \
} \
\
void resize_##T(T* ptr, int length) {\
    if (ptr->length != length) {\
        if (!length) {\
            dispose_##T(ptr);\
        } else if (!ptr->value) {\
            initialize_##T(ptr, length);\
        } else {\
            type* new_memory = zox_realloc(ptr->value, length * sizeof(type));\
            if (!new_memory) {\
                zox_log_error("Failure with realloc");\
            } else {\
                ptr->value = new_memory;\
                ptr->length = length;\
            }\
        }\
    }\
} \
\
byte add_to_##T(T *ptr, type data) { \
    spin_lock(&ptr->lock); \
    int new_length = ptr->length + 1; \
    type* new_value = ptr->value \
        ? zox_realloc(ptr->value, new_length * sizeof(type)) \
        : zox_malloc(new_length * sizeof(type)); \
    if (!new_value) { \
        zox_log_error("malloc failed in add_to_" #T); \
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
    zoxd(T)\
    zox_define_hooks(T);
