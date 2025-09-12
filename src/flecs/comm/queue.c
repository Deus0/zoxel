#define zoxc_queue(T, T2, initial_capacity) \
typedef struct { \
    T2* ptr; \
    size_t count; \
    size_t capacity; \
    SpinLock lock; \
} T; \
zoxc_custom(T); \
\
static void i_##T(T* q) { \
    q->ptr = malloc(sizeof(T2) * (initial_capacity)); \
    q->count = 0; \
    q->capacity = (initial_capacity); \
    q->lock = SPINLOCK_INIT; \
} \
\
static void d_##T(T* q) { \
    if (q->ptr) free(q->ptr); \
    q->ptr = NULL; \
    q->count = 0; \
    q->capacity = 0; \
} \
\
static void a_##T(T* q, T2 item) { \
    if (!q->ptr || q->count == q->capacity) { \
        size_t new_cap = q->capacity ? q->capacity * 2 : 1; \
        q->ptr = q->ptr ? realloc(q->ptr, sizeof(T2) * new_cap) \
                        : malloc(sizeof(T2) * new_cap); \
        q->capacity = new_cap; \
    } \
    q->ptr[q->count++] = item; \
} \
\
static T2 r_##T(T* q) { \
    if (q->count == 0) { \
        T2 empty = {0}; \
        return empty; \
    } \
    return q->ptr[--q->count]; \
} \
\
ECS_CTOR(T, ptr, { i_##T(ptr); }) \
ECS_DTOR(T, ptr, { d_##T(ptr); }) \
ECS_MOVE(T, dst, src, { \
    *dst = *src; \
    src->ptr = NULL; \
    src->count = 0; \
    src->capacity = 0; \
}) \
ECS_COPY(T, dst, src, { \
    if (dst->ptr) free(dst->ptr); \
    if (src->ptr) { \
        dst->ptr = malloc(src->capacity * sizeof(T2)); \
        memcpy(dst->ptr, src->ptr, src->count * sizeof(T2)); \
        dst->count = src->count; \
        dst->capacity = src->capacity; \
    } else { \
        dst->ptr = NULL; \
        dst->count = 0; \
        dst->capacity = 0; \
    } \
})



#define zoxd_queue(T)\
    zoxd(T);\
    zox_define_hooks(T);
