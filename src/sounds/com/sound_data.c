#define MAX_SOUND_DATA (44100 * 3) // max time for sound 4 seconds

#ifndef zox_disable_sound_pool

#define MAX_SOUND_INSTANCES 64

static float sound_pool[MAX_SOUND_INSTANCES][MAX_SOUND_DATA];
static byte sound_pool_used[MAX_SOUND_INSTANCES] = {0};

float* soundpool_alloc(uint length) {
    if (length > MAX_SOUND_INSTANCES) {
        zox_loge("soundpool alloc length too high");
        return NULL;
    }
    for (int i = 0; i < MAX_SOUND_INSTANCES; i++) {
        if (!sound_pool_used[i]) {
            sound_pool_used[i] = 1;
            return sound_pool[i];
        }
    }
    return NULL; // no free chunk
}

void soundpool_free(float* ptr) {
    for (int i = 0; i < MAX_SOUND_INSTANCES; i++) {
        if (sound_pool[i] == ptr) {
            sound_pool_used[i] = 0;
            return;
        }
    }
}

#else

void soundpool_free(float* ptr) {
    if (ptr) {
        free(ptr);
    }
}

float* soundpool_alloc(uint length) {
    return (float*) malloc(length * sizeof(float));
}

#endif

typedef struct {
    uint length;
    float* value;   // point to our sound pool
} SoundData;
zoxc_custom(SoundData);

/*ECS_DTOR(SoundData, ptr, {
    if (ptr->value) {
        soundpool_free(ptr->value);
    }
})*/

void on_destroyed_SoundData(iter *it) {
    byte dbg_log = 0;
    zox_sys_begin();
    zox_sys_out(SoundData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(SoundData, component);
        if (!component->value) {
            continue;
        }
        if (dbg_log) {
            zox_log("SoundData Destroy [%i]", component->value);
        }
        soundpool_free(component->value);
        component->value = 0;
    }
}

