#include "windows.c"
#include "linux.c"
#include "link.c"
#include "path.c"
#include "android_assets_db.c"
#include "android_decompress.c"
#include "android.c"
#include "directory.c"
#include "dir.c"
#include "dirs.c"
#include "save_dir.c"
#include "saves.c"
#include "io_macros.c"
#include "realm_save.c"
#include "game.c"

static int ends_with(const char *str, const char *suffix) {
    if (!str || !suffix) return 0;
    size_t len_str = strlen(str);
    size_t len_suf = strlen(suffix);
    if (len_suf > len_str) return 0;
    return strcmp(str + (len_str - len_suf), suffix) == 0;
}
