// Prints the command line flags and exits, for -h / --help
//  - keep in sync with doc/commands.md, which has the details and the source of each flag
//  - printf, as release builds strip zox_log
//  - runs before the window is made (terminal hooks run before initialize_sdl), so it exits directly
void print_help_menu(const char* arg0) {
    printf(
        "\n"
        "Zoxel\n"
        "\n"
        "  Usage: %s [options]\n"
        "\n"
        "  Window\n"
        "    -w  --windowed          windowed mode\n"
        "    -f  --fullscreen        fullscreen, overrides the saved setting\n"
        "    -g  --halfscreen        half screen window\n"
        "    -s  --splitscreen       split screen local coop\n"
        "        --nodecor           no window decorations\n"
        "        --background        hidden window, no sound (used by tests)\n"
        "        --1080p             a 1920x1080 window (tests: screenshots at the game's size)\n"
        "        --nohud             the ui isn't drawn (tests: screenshots of the world)\n"
        "    -z  --headless          no window\n"
        "        --vsync             vsync on (--enablevsync)\n"
        "        --novsync           vsync off (--disablevsync)\n"
        "    -tu --tinyui            ui scale 1\n"
        "    -su --smallui           ui scale 2\n"
        "\n"
        "  Graphics\n"
        "        --gles              opengl es, skips the desktop gl check\n"
        "        --es                opengl es (--opengles)\n"
        "        --openglcore        desktop opengl core\n"
        "    -v  --vulkan            vulkan backend (experimental)\n"
        "        --verbose           verbose logs, i.e. the gl version and renderer\n"
        "\n"
        "  Game\n"
        "        --autostart         skip the menus, start a new game\n"
        "        --tiny              terrain stream distance 3, height 3\n"
        "        --medium            terrain stream distance 6, height 3\n"
        "        --large             terrain stream distance 12, height 4\n"
        "        --freeroam          free camera, not attached to the character\n"
        "        --nonpcs            no npcs (--nonpc)\n"
        "        --nohumans          no humanoid npcs (villagers)\n"
        "        --nominivoxes       no block voxes (--noblockvoxes)\n"
        "        --nosaves           saves go to a temp folder, real saves untouched\n"
        "\n"
        "  Audio\n"
        "        --nosounds          no sound effects (--nosound)\n"
        "        --nomusic           no music\n"
        "\n"
        "  Engine\n"
        "        --fps <n>           target frame rate\n"
        "        --singlethread      run systems on one thread\n"
        "    -p  --profiler          flecs profiler\n"
        "\n"
        "  Networking\n"
        "    -x  --server            server mode\n"
        "        --client            client mode\n"
        "        --online            networking on\n"
        "        --offline           networking off\n"
        "        --lan               connect to the local ip\n"
        "\n"
        "  Tests and Debugging (usually through bash bsh/tst.sh)\n"
        "        --tst <name>        run a test, exit code 0 on pass\n"
        "        --tst list          list the tests\n"
        "        --tst-errors <n>    fail a test after n logged errors (default 100)\n"
        "        --tst-timescale <f> scale test timeouts\n"
        "        --blackplague       damage every npc every 5s (stress test)\n"
        "\n"
        "    -h  --help              print this help\n"
        "\n"
        "  More in doc/commands.md\n"
        "\n",
        arg0);
    fflush(stdout);
}

void process_arguments_help(ecs *world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "-h") == 0 || strcmp(args[i], "--help") == 0) {
            print_help_menu(args[0]);
            exit(EXIT_SUCCESS);
        }
    }
}
