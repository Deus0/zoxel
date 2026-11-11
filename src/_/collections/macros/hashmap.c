// default hash is uint
// NOTE: only the hash of a key is stored, keys with the same hash are the same entry
//  - int3 chunk positions are unique within a streaming window (tst hashmaps)
#define zox_hashmap(name, type, type_zero, key_type, hash_type, convert_to_hash)\
\
typedef struct name##_pair name##_pair;\
\
struct name##_pair {\
    hash_type key;\
    type value;\
    struct name##_pair* next;\
};\
\
typedef struct {\
    hash_type size;\
    name##_pair **data;\
    zox_lock lock;\
} name;\
\
static inline void create_lock_##name(name *ptr) {\
    if (hashmap_safety_locks) {\
        zox_lock_init(&ptr->lock);\
    }\
}\
\
static inline void destroy_lock_##name(name *ptr) {\
    if (hashmap_safety_locks) {\
        zox_lock_destroy(&ptr->lock);\
    }\
}\
\
static inline void write_lock_##name(const name *ptr) {\
    if (hashmap_safety_locks) {\
        zox_lock_write(&ptr->lock);\
    }\
}\
\
static inline void write_unlock_##name(const name *ptr) {\
    if (hashmap_safety_locks) {\
        zox_unlock_write(&ptr->lock);\
    }\
}\
\
static inline void read_lock_##name(const name *ptr) {\
    if (hashmap_safety_locks) {\
        zox_lock_read(&ptr->lock);\
    }\
}\
\
static inline void read_unlock_##name(const name *ptr) {\
    if (hashmap_safety_locks) {\
        zox_unlock_read(&ptr->lock);\
    }\
}\
\
hash_type name##_hash(hash_type key, hash_type size) {\
    if (size == 0) {\
        return -1;\
    }\
    /* NOTE: mixes the bits so packed keys spread over the buckets (murmur3 finalizer) */\
    uint mixed = (uint) key;\
    mixed ^= mixed >> 16;\
    mixed *= 0x85ebca6bu;\
    mixed ^= mixed >> 13;\
    mixed *= 0xc2b2ae35u;\
    mixed ^= mixed >> 16;\
    return mixed % size;\
}\
\
name* create_##name(hash_type size) {\
    if (size == 0) {\
        zox_log("! cannot have a zero hashmap [%s]", #name)\
        return NULL;\
    }\
    name* map = zox_malloc(sizeof(name));\
    if (!map) {\
        zox_loge("error creating hashmap [%s]\n", #name)\
        return NULL;\
    }\
    map->size = size;\
    /*calloc zeroes out data*/\
    map->data = calloc(size, sizeof(name##_pair*));\
    create_lock_##name(map);\
    return map;\
}\
\
void name##_add(name* map, key_type key_raw, type value) {\
    if (!map || !map->data || map->size == 0) {\
        return;\
    }\
    write_lock_##name(map);\
    hash_type key = convert_to_hash(key_raw);\
    hash_type index = name##_hash(key, map->size);\
    /* NOTE: an existing key is replaced, a duplicate made remove leave the old value behind */\
    name##_pair* existing = map->data[index];\
    uint existing_checks = 0;\
    while (existing != NULL && existing_checks < max_safety_checks_hashmap) {\
        if (existing->key == key) {\
            existing->value = value;\
            write_unlock_##name(map);\
            return;\
        }\
        existing = existing->next;\
        existing_checks++;\
    }\
    name##_pair* pair = zox_malloc(sizeof(name##_pair));\
    pair->key = key;\
    pair->value = value;\
    pair->next = map->data[index];\
    map->data[index] = pair;\
    write_unlock_##name(map);\
}\
\
type name##_get(name* map, key_type key_raw) {\
    if (!map || !map->data || map->size == 0) {\
        return type_zero;\
    }\
    read_lock_##name(map);\
    hash_type key = convert_to_hash(key_raw);\
    hash_type index = name##_hash(key, map->size);\
    name##_pair* pair = map->data[index];\
    uint checks = 0;\
    type value = type_zero;\
    while (pair != NULL && checks < max_safety_checks_hashmap) {\
        if (pair->key == key) {\
            value = pair->value;\
            break;\
        }\
        pair = pair->next;\
        checks++;\
    }\
    read_unlock_##name(map);\
    return value;\
}\
\
byte name##_has(name* map, key_type key_raw) {\
    if (!map || !map->data || map->size == 0) {\
        return 1;\
    }\
    read_lock_##name(map);\
    hash_type key = convert_to_hash(key_raw);\
    hash_type index = name##_hash(key, map->size);\
    name##_pair* pair = map->data[index];\
    uint checks = 0;\
    while (pair != NULL && checks < max_safety_checks_hashmap) {\
        if (pair->key == key) {\
            read_unlock_##name(map);\
            return 1;\
        }\
        pair = pair->next;\
        checks++;\
    }\
    read_unlock_##name(map);\
    return 0;\
}\
\
void name##_remove(name* map, key_type key_raw) {\
    if (!map || !map->data || map->size == 0) {\
        return;\
    }\
    write_lock_##name(map);\
    hash_type key = convert_to_hash(key_raw);\
    hash_type index = name##_hash(key, map->size);\
    name##_pair* pair = map->data[index];\
    name##_pair* prev_pair = NULL;\
    uint checks = 0;\
    while (pair != NULL && checks < max_safety_checks_hashmap) {\
        if (pair->key == key) {\
            if (prev_pair == NULL) {\
                map->data[index] = pair->next;\
            } else {\
                prev_pair->next = pair->next;\
            }\
            zox_free(pair);\
            break;\
        }\
        prev_pair = pair;\
        pair = pair->next;\
        checks++;\
    }\
    write_unlock_##name(map);\
}\
\
void name##_dispose(name* map) {\
    if (!map || map->size == 0 || !map->data) {\
        return;\
    }\
    /*write_lock_##name(map);*/\
    for (size_t i = 0; i < map->size; i++) {\
        name##_pair* pair = map->data[i];\
        uint checks = 0;\
        while (pair != NULL && checks < max_safety_checks_hashmap) {\
            name##_pair* next_pair = pair->next;\
            zox_free(pair);\
            pair = next_pair;\
            checks++;\
        }\
    }\
    zox_free(map->data);\
    /*write_unlock_##name(map);*/\
    destroy_lock_##name(map);\
    zox_free(map);\
}\
\
int count_##name(name* map) {\
    if (!map || !map->size) {\
        return 0;\
    }\
    read_lock_##name(map);\
    size_t count = 0;\
    for (size_t i = 0; i < map->size; i++) {\
        name##_pair* pair = map->data[i];\
        uint checks = 0;\
        while (pair != NULL && checks < max_safety_checks_hashmap) {\
            name##_pair* next_pair = pair->next;\
            count++;\
            pair = next_pair;\
            checks++;\
        }\
    }\
    read_unlock_##name(map);\
    return count;\
}
