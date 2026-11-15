// a realm's info, realm.txt in its save folder: its seed, name, seconds played, when last played (unix time)
//  - lines of "key value"; older saves have only seed.dat (load_realm_info reads its seed, the folder is its name)
#define realm_name_length 32
typedef struct {
    uint seed;
    char name[realm_name_length];
    uint played;
    long last;
} RealmInfo;

byte load_realm_info(const char* folder, RealmInfo* info) {
    *info = (RealmInfo) { 0 };
    char* path = join_paths(folder, "realm.txt");
    FILE* file = path ? fopen(path, "r") : NULL;
    free(path);
    if (!file) {
        SaveRealm old = { 0 };
        const byte loaded = load_file_struct(folder, "seed.dat", &old, sizeof(SaveRealm));
        info->seed = old.seed;
        return loaded;
    }
    char line[128];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        if (!strncmp(line, "seed ", 5)) {
            info->seed = (uint) strtoul(line + 5, NULL, 10);
        } else if (!strncmp(line, "name ", 5)) {
            snprintf(info->name, sizeof(info->name), "%s", line + 5);
        } else if (!strncmp(line, "played ", 7)) {
            info->played = (uint) strtoul(line + 7, NULL, 10);
        } else if (!strncmp(line, "last ", 5)) {
            info->last = strtol(line + 5, NULL, 10);
        }
    }
    fclose(file);
    return 1;
}

byte save_realm_info(const char* folder, const RealmInfo* info) {
    char* path = join_paths(folder, "realm.txt");
    FILE* file = path ? fopen(path, "w") : NULL;
    free(path);
    if (!file) {
        return 0;
    }
    fprintf(file, "seed %u\nname %s\nplayed %u\nlast %ld\n", info->seed, info->name, info->played, info->last);
    fclose(file);
    return 1;
}

// time played as text: 2h 5m, 5m, or under a minute
void realm_played_text(uint seconds, char* text, int size) {
    if (seconds >= 3600) {
        snprintf(text, size, "%uh %um", seconds / 3600, (seconds % 3600) / 60);
    } else if (seconds >= 60) {
        snprintf(text, size, "%um", seconds / 60);
    } else {
        snprintf(text, size, "under a minute");
    }
}

// a name from a seed, two or three syllables (a new realm's, before the player names it)
void realm_default_name(uint seed, char* name, int size) {
    const char* syllables[] = { "zo", "ka", "ri", "mel", "dor", "an", "vel", "ith", "su", "gar", "eth", "lo", "nim", "ta" };
    const int count = sizeof(syllables) / sizeof(syllables[0]);
    uint s = seed * 2654435761u;
    const int parts = 2 + s % 2;
    name[0] = 0;
    for (int i = 0; i < parts; i++) {
        s = s * 1103515245u + 12345u;
        strncat(name, syllables[(s >> 16) % count], size - strlen(name) - 1);
    }
    if (name[0] >= 'a' && name[0] <= 'z') {
        name[0] -= 32;
    }
}

// when this session's realm was entered, its played time adds the time since (realm_info_played)
double realm_played_since = 0;

// adds the time played since the realm was entered to its info, marks it last played now
void realm_info_played(const char* folder, double now) {
    RealmInfo info;
    load_realm_info(folder, &info);
    if (realm_played_since > 0 && now > realm_played_since) {
        info.played += (uint) (now - realm_played_since);
    }
    info.last = (long) time(NULL);
    save_realm_info(folder, &info);
    realm_played_since = now;
}
