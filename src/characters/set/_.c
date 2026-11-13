byte disable_npcs = 0;
byte max_npcs = 0;     // --maxnpcs: every chunk spawns the most npcs, a worst case to profile
byte is_characters_instanced = 0;

void process_arguments_characters(ecs *world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "--nonpc") == 0 ||
            strcmp(args[i], "--nonpcs") == 0)
        {
            disable_npcs = 1;
        }
        if (strcmp(args[i], "--maxnpcs") == 0) {
            max_npcs = 1;
        }
        // --nohumans: no humanoid npcs (villagers), i.e. to see what they cost (end_game_cleanup's memory)
        if (strcmp(args[i], "--nohumans") == 0) {
            zox_no_humanoids = 1;
        }
    }
}
