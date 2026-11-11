// NOTE: Assume Max Depth
typedef struct {
    int2 position;  // grid2 position
    int distance;   // streamer distance
    entity tunk;    // update tunks
    byte level;     // streamer level
    double time;    // time added to queue
    double queued;  // time first queued, kept when re-queued (time isn't), for aging
} TerrainSpawnUpdate;
zoxc_custom(TerrainSpawnQueue);

typedef struct {
    TerrainSpawnUpdate* ptr;
    size_t count;
    size_t capacity;
    // Open-addressed hash set of positions currently in the queue.
    int2* hash_keys;
    byte* hash_state; // 0 = empty, 1 = used, 2 = tombstone
    size_t hash_capacity;
    size_t hash_count;
    size_t hash_tombstones; // removed slots, lookups probe past them until a rehash
    spinlock lock;
} TerrainSpawnQueue;

static inline uint hash_terrain_spawn_position(int2 position) {
    uint x = (uint) position.x;
    uint y = (uint) position.y;

    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;

    y ^= y >> 16;
    y *= 0x7feb352dU;
    y ^= y >> 15;
    y *= 0x846ca68bU;
    y ^= y >> 16;

    return x ^ (y * 0x9e3779b9U);
}

static inline byte terrain_spawn_position_equal(
    int2 a,
    int2 b)
{
    return a.x == b.x && a.y == b.y;
}

static void init_TerrainSpawnQueueHash(
    TerrainSpawnQueue* q,
    size_t capacity)
{
    if (capacity < 16) {
        capacity = 16;
    }

    // Hash capacity must remain a power of two.
    size_t hash_capacity = 1;
    while (hash_capacity < capacity) {
        hash_capacity <<= 1;
    }

    q->hash_keys =
    zox_malloc(sizeof(int2) * hash_capacity);

    q->hash_state =
    zox_malloc(sizeof(byte) * hash_capacity);

    memset(
        q->hash_state,
        0,
        sizeof(byte) * hash_capacity);

    q->hash_capacity = hash_capacity;
    q->hash_count = 0;
    q->hash_tombstones = 0;
}

static void rehash_TerrainSpawnQueue(
    TerrainSpawnQueue* q,
    size_t new_capacity)
{
    size_t hash_capacity = 1;

    while (hash_capacity < new_capacity) {
        hash_capacity <<= 1;
    }

    int2* new_keys =
    zox_malloc(sizeof(int2) * hash_capacity);

    byte* new_state =
    zox_malloc(sizeof(byte) * hash_capacity);

    memset(
        new_state,
        0,
        sizeof(byte) * hash_capacity);

    const size_t new_mask = hash_capacity - 1;

    for (size_t i = 0; i < q->hash_capacity; i++) {
        if (q->hash_state[i] != 1) {
            continue;
        }

        int2 position = q->hash_keys[i];
        size_t index =
        hash_terrain_spawn_position(position) & new_mask;

        while (new_state[index] == 1) {
            index = (index + 1) & new_mask;
        }

        new_state[index] = 1;
        new_keys[index] = position;
    }

    if (q->hash_keys) {
        zox_free(q->hash_keys);
    }

    if (q->hash_state) {
        zox_free(q->hash_state);
    }

    q->hash_keys = new_keys;
    q->hash_state = new_state;
    q->hash_capacity = hash_capacity;
    q->hash_tombstones = 0;
}

static byte inside_TerrainSpawnQueue(
    const TerrainSpawnQueue* q,
    int2 position)
{
    if (!q->hash_capacity) {
        return 0;
    }

    const size_t mask = q->hash_capacity - 1;
    size_t index =
    hash_terrain_spawn_position(position) & mask;

    while (q->hash_state[index] != 0) {
        if (q->hash_state[index] == 1 &&
            terrain_spawn_position_equal(
                q->hash_keys[index],
                position))
        {
            return 1;
        }

        index = (index + 1) & mask;
    }

    return 0;
}

static void insert_TerrainSpawnQueueHash(
    TerrainSpawnQueue* q,
    int2 position)
{
    // Grow at ~70% load. Tombstones count too, lookups only stop at empty slots,
    // so a table full of tombstones would never end a probe
    if (!q->hash_capacity) {
        init_TerrainSpawnQueueHash(q, 16);
    } else if ((q->hash_count + q->hash_tombstones + 1) * 10 >=
        q->hash_capacity * 7)
    {
        // only grow if the live keys need it, otherwise rehash in place to drop tombstones
        const size_t grow = (q->hash_count + 1) * 10 >= q->hash_capacity * 5;
        rehash_TerrainSpawnQueue(
            q,
            grow ? q->hash_capacity * 2 : q->hash_capacity);
    }

    const size_t mask = q->hash_capacity - 1;
    size_t index =
    hash_terrain_spawn_position(position) & mask;

    while (q->hash_state[index] == 1) {
        if (terrain_spawn_position_equal(
            q->hash_keys[index],
            position))
        {
            return;
        }

        index = (index + 1) & mask;
    }

    q->hash_keys[index] = position;
    q->hash_state[index] = 1;
    q->hash_count++;
}

static void remove_TerrainSpawnQueueHash(
    TerrainSpawnQueue* q,
    int2 position)
{
    if (!q->hash_capacity) {
        return;
    }

    const size_t mask = q->hash_capacity - 1;
    size_t index =
    hash_terrain_spawn_position(position) & mask;

    while (q->hash_state[index] != 0) {
        if (q->hash_state[index] == 1 &&
            terrain_spawn_position_equal(
                q->hash_keys[index],
                position))
        {
            q->hash_state[index] = 2; // tombstone
            q->hash_count--;
            q->hash_tombstones++;
            return;
        }

        index = (index + 1) & mask;
    }
}

static void i_TerrainSpawnQueue(
    TerrainSpawnQueue* q)
{
    q->ptr =
    zox_malloc(sizeof(TerrainSpawnUpdate));

    q->count = 0;
    q->capacity = 1;

    q->hash_keys = NULL;
    q->hash_state = NULL;
    q->hash_capacity = 0;
    q->hash_count = 0;
    q->hash_tombstones = 0;

    spinlock_init(&q->lock);

    init_TerrainSpawnQueueHash(q, 16);
}

static void d_TerrainSpawnQueue(
    TerrainSpawnQueue* q)
{
    if (q->ptr) {
        zox_free(q->ptr);
    }

    if (q->hash_keys) {
        zox_free(q->hash_keys);
    }

    if (q->hash_state) {
        zox_free(q->hash_state);
    }

    q->ptr = NULL;
    q->count = 0;
    q->capacity = 0;

    q->hash_keys = NULL;
    q->hash_state = NULL;
    q->hash_capacity = 0;
    q->hash_count = 0;
    q->hash_tombstones = 0;
}

// Empties the queue, keep its positions hash in sync, a position left in it can never be queued again
static void clear_TerrainSpawnQueue(
    TerrainSpawnQueue* q)
{
    q->count = 0;
    if (q->hash_state && q->hash_capacity) {
        memset(
            q->hash_state,
            0,
            sizeof(byte) * q->hash_capacity);
    }
    q->hash_count = 0;
    q->hash_tombstones = 0;
}

static byte add_TerrainSpawnQueue(
    TerrainSpawnQueue* q,
    TerrainSpawnUpdate item)
{
    // Already waiting in the queue.
    if (inside_TerrainSpawnQueue(q, item.position)) {
        return 0;
    }

    if (!q->ptr ||
        q->count == q->capacity)
    {
        size_t new_capacity =
        q->capacity
        ? q->capacity * 2
        : 1;

        q->ptr = q->ptr
        ? zox_realloc(
            q->ptr,
            sizeof(TerrainSpawnUpdate) *
            new_capacity)
        : zox_malloc(
            sizeof(TerrainSpawnUpdate) *
            new_capacity);

        q->capacity = new_capacity;
    }

    q->ptr[q->count++] = item;
    insert_TerrainSpawnQueueHash(
        q,
        item.position);

    return 1;
}

/*static TerrainSpawnUpdate remove_TerrainSpawnQueue(
    TerrainSpawnQueue* q)
{
    if (q->count == 0) {
        TerrainSpawnUpdate empty = {0};
        return empty;
    }

    TerrainSpawnUpdate item =
    q->ptr[--q->count];

    remove_TerrainSpawnQueueHash(
        q,
        item.position);

    return item;
}*/

ECS_CTOR(
    TerrainSpawnQueue,
    ptr,
    {
        i_TerrainSpawnQueue(ptr);
    }
)

ECS_DTOR(
    TerrainSpawnQueue,
    ptr,
    {
        d_TerrainSpawnQueue(ptr);
    }
)

ECS_MOVE(
    TerrainSpawnQueue,
    dst,
    src,
    {
        *dst = *src;

        src->ptr = NULL;
        src->count = 0;
        src->capacity = 0;

        src->hash_keys = NULL;
        src->hash_state = NULL;
        src->hash_capacity = 0;
        src->hash_count = 0;
        src->hash_tombstones = 0;
    }
)

ECS_COPY(
    TerrainSpawnQueue,
    dst,
    src,
    {
        if (dst->ptr) {
            zox_free(dst->ptr);
        }

        if (dst->hash_keys) {
            zox_free(dst->hash_keys);
        }

        if (dst->hash_state) {
            zox_free(dst->hash_state);
        }

        dst->count = src->count;
        dst->capacity = src->capacity;

        if (src->ptr && src->capacity) {
            dst->ptr =
            zox_malloc(
                src->capacity *
                sizeof(TerrainSpawnUpdate));

            memcpy(
                dst->ptr,
                src->ptr,
                src->count *
                sizeof(TerrainSpawnUpdate));
        } else {
            dst->ptr = NULL;
            dst->count = 0;
            dst->capacity = 0;
        }

        dst->hash_capacity = src->hash_capacity;
        dst->hash_count = src->hash_count;
        dst->hash_tombstones = src->hash_tombstones;

        if (src->hash_capacity) {
            dst->hash_keys =
            zox_malloc(
                src->hash_capacity *
                sizeof(int2));

            dst->hash_state =
            zox_malloc(
                src->hash_capacity *
                sizeof(byte));

            memcpy(
                dst->hash_keys,
                src->hash_keys,
                src->hash_capacity *
                sizeof(int2));

            memcpy(
                dst->hash_state,
                src->hash_state,
                src->hash_capacity *
                sizeof(byte));
        } else {
            dst->hash_keys = NULL;
            dst->hash_state = NULL;
            dst->hash_capacity = 0;
            dst->hash_count = 0;
        }

        spinlock_init(&dst->lock);
    }
)

void define_terrain_spawn_queue(ecs* world) {
    zoxd_queue(TerrainSpawnQueue);
}

static TerrainSpawnUpdate* get_TerrainSpawnQueue(
    TerrainSpawnQueue* q,
    int2 position)
{
    if (!q->hash_capacity) {
        return NULL;
    }
    const size_t mask = q->hash_capacity - 1;
    size_t index =
    hash_terrain_spawn_position(position) & mask;
    while (q->hash_state[index] != 0) {
        if (q->hash_state[index] == 1 &&
            terrain_spawn_position_equal(
                q->hash_keys[index],
                position))
        {
            for (size_t i = 0; i < q->count; i++) {
                if (terrain_spawn_position_equal(
                    q->ptr[i].position,
                    position))
                {
                    return &q->ptr[i];
                }
            }
            return NULL;
        }

        index = (index + 1) & mask;
    }

    return NULL;
}

static byte add_or_set_TerrainSpawnQueue(
    TerrainSpawnQueue* q,
    TerrainSpawnUpdate item)
{
    TerrainSpawnUpdate* existing =
    get_TerrainSpawnQueue(q, item.position);
    if (existing) {
        // NOTE: Higher levels win so region-only updates cannot overwrite Tunk spawn updates.
        if (item.level < existing->level) {
            return 0;
        }
        // NOTE: Keep the closest distance update from the same update frame
        if (item.time == existing->time &&
            item.distance >= existing->distance)
        {
            return 0;
        }
        item.queued = existing->queued;
        *existing = item;
        return 0;
    }
    item.queued = item.time;
    return add_TerrainSpawnQueue(q, item);
}

static int get_closest_TerrainSpawnQueue_distance(
    TerrainSpawnQueue* q)
{
    if (!q->count) {
        return terrain_lod_far + 1;
    }
    int closest = q->ptr[0].distance;
    for (size_t i = 1; i < q->count; i++) {
        if (q->ptr[i].distance < closest) {
            closest = q->ptr[i].distance;
        }
    }
    return closest;
}

static TerrainSpawnUpdate remove_closest_TerrainSpawnQueue(
    TerrainSpawnQueue* q)
{
    if (q->count == 0) {
        TerrainSpawnUpdate empty = {0};
        return empty;
    }

    int closest_distance =
        get_closest_TerrainSpawnQueue_distance(q);

    for (size_t i = 0; i < q->count; i++) {
        if (q->ptr[i].distance != closest_distance) {
            continue;
        }

        TerrainSpawnUpdate item = q->ptr[i];
        size_t last = q->count - 1;

        if (i != last) {
            q->ptr[i] = q->ptr[last];
        }

        q->count--;

        remove_TerrainSpawnQueueHash(
            q,
            item.position);

        return item;
    }

    TerrainSpawnUpdate empty = {0};
    return empty;
}

// Picks the next update without starving any, so streaming doesn't depend on frame rate
//  - kills first (past terrain_lod_far), they're cheap and free memory
//  - then any update waiting longer than max_wait, oldest first
//  - otherwise the closest, so terrain near the streamer builds first
//  - closest only used to starve far updates (unloads, npc despawns) while the streamer kept moving
static TerrainSpawnUpdate remove_next_TerrainSpawnQueue(
    TerrainSpawnQueue* q,
    double now,
    double max_wait)
{
    if (q->count == 0) {
        TerrainSpawnUpdate empty = {0};
        return empty;
    }
    size_t closest = 0;
    size_t oldest = 0;
    size_t kill = q->count;
    for (size_t i = 0; i < q->count; i++) {
        const TerrainSpawnUpdate* u = &q->ptr[i];
        if (u->distance > terrain_lod_far) {
            kill = i;
            break;
        }
        if (u->distance < q->ptr[closest].distance) {
            closest = i;
        }
        if (u->queued < q->ptr[oldest].queued) {
            oldest = i;
        }
    }
    size_t pick = closest;
    if (kill < q->count) {
        pick = kill;
    } else if (now - q->ptr[oldest].queued > max_wait) {
        pick = oldest;
    }
    TerrainSpawnUpdate item = q->ptr[pick];
    size_t last = q->count - 1;
    if (pick != last) {
        q->ptr[pick] = q->ptr[last];
    }
    q->count--;
    remove_TerrainSpawnQueueHash(q, item.position);
    return item;
}