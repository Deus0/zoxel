#define zoxc_queue(T, T2, initial_capacity) \
typedef struct { \
    T2* ptr; \
    size_t count; \
    size_t capacity; \
    spinlock lock; \
} T; \
zoxc_custom(T); \
\
static void i_##T(T* q) { \
    q->ptr = zox_malloc(sizeof(T2) * (initial_capacity)); \
    q->count = 0; \
    q->capacity = (initial_capacity); \
    spinlock_init(&q->lock); \
} \
\
static void dispose_##T(T* q) { \
    spin_lock(&q->lock); \
    if (q->ptr) {\
        zox_free(q->ptr); \
    } \
    q->ptr = NULL; \
    q->count = 0; \
    q->capacity = 0; \
    spin_unlock(&q->lock); \
} \
\
static void a_##T(T* q, T2 item) { \
    if (!q->ptr || q->count == q->capacity) { \
        size_t new_cap = q->capacity ? q->capacity * 2 : 1; \
        q->ptr = q->ptr ? zox_realloc(q->ptr, sizeof(T2) * new_cap) \
                        : zox_malloc(sizeof(T2) * new_cap); \
        q->capacity = new_cap; \
    } \
    q->ptr[q->count++] = item; \
} \
\
ECS_CTOR(T, ptr, { i_##T(ptr); }) \
\
ECS_DTOR(T, ptr, { dispose_##T(ptr); }) \
\
ECS_MOVE(T, dst, src, { \
    dst->ptr = src->ptr; \
    dst->count = src->count; \
    dst->capacity = src->capacity; \
    spinlock_init(&dst->lock); \
    src->ptr = NULL; \
    src->count = 0; \
    src->capacity = 0; \
}) \
\
ECS_COPY(T, dst, src, { \
    spin_lock((spinlock*)&src->lock); \
    spin_lock(&dst->lock); \
    if (dst->ptr) zox_free(dst->ptr); \
    if (src->ptr) { \
        dst->ptr = zox_malloc(src->capacity * sizeof(T2)); \
        if (dst->ptr) { \
            memcpy(dst->ptr, src->ptr, src->count * sizeof(T2)); \
            dst->count = src->count; \
            dst->capacity = src->capacity; \
        } else { \
            dst->count = 0; \
            dst->capacity = 0; \
        } \
    } else { \
        dst->ptr = NULL; \
        dst->count = 0; \
        dst->capacity = 0; \
    } \
    spin_unlock(&dst->lock); \
    spin_unlock((spinlock*)&src->lock); \
})

#define zoxc_queue_remove(T, T2) \
static T2 remove_##T(T* q) { \
    if (q->count == 0) { \
        T2 empty = {0}; \
        return empty; \
    } \
    return q->ptr[--q->count]; \
}

#define zoxc_queue_remove_at(T, T2) \
static void remove_at_##T(T* q, size_t index) { \
    if (index >= q->count) return; \
    \
    if (index < q->count - 1) { \
        memmove(&q->ptr[index], \
        &q->ptr[index + 1], \
        (q->count - index - 1) * sizeof(T2)); \
    } \
    \
    q->count--; \
}


#define zoxd_queue(T)\
    zoxd(T);\
    zox_define_hooks(T);
