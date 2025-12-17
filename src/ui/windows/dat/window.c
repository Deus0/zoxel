// Used for spawning windows
typedef {
    int2 position;
    int2 size;
} struct WindowSpawnData;

WindowSpawnData create_default_window_spawn_data() {
    WindowSpawnData spawn_data;

    return spawn_data;
}