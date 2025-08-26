void get_save_directory(
    const char *game,
    char *path, size_t size
) {
#ifdef zox_windows
    const char *home_directory = getenv("USERPROFILE");
    if (home_directory) {
        snprintf(path, size, "%s\\AppData\\Local\\%s", home_directory, game);
    } else {
        zox_log(" ! home_directory null [get_save_directory]")
    }
    /*char home_directory[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, home_directory))) {
        snprintf(path, size, "%s\\%s", home_directory, game);
    } else {
        zox_log(" ! home_directory null [get_save_directory]")
    }*/
#elif defined(zox_android)
    // Android-specific code
    JNIEnv* env = (JNIEnv*) SDL_AndroidGetJNIEnv();
    jobject activity = (jobject) SDL_AndroidGetActivity();

    jclass context_class = (*env)->GetObjectClass(env, activity);
    jmethodID get_files_dir = (*env)->GetMethodID(env, context_class, "getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;");

    jobject file = (*env)->CallObjectMethod(env, activity, get_files_dir, NULL);
    jclass file_class = (*env)->GetObjectClass(env, file);
    jmethodID get_path = (*env)->GetMethodID(env, file_class, "getPath", "()Ljava/lang/String;");

    jstring path_string = (jstring)(*env)->CallObjectMethod(env, file, get_path);
    const char *path_cstr = (*env)->GetStringUTFChars(env, path_string, NULL);

    snprintf(path, size, "%s/%s", path_cstr, game);

    (*env)->ReleaseStringUTFChars(env, path_string, path_cstr);
#else
    const char *home_directory = getenv("HOME");
    if (home_directory) {
        snprintf(path, size, "%s/.local/share/%s", home_directory, game);
    } else {
        zox_log(" ! home_directory null [get_save_directory]")
    }
#endif
}

// 1 on existing
byte create_new_save_directory(const char *game) {
    char path[max_path_characters];
    get_save_directory(game, path, sizeof(path));
    if (!has_path_directory(path)) {
        zox_log_io(" + creating new save directory [%s]", path)
#ifdef zox_windows
        if (mkdir(path) == 0) {
            return 1;
        } else {
            zox_log(" ! error creating directory");
        }
#else
        if (mkdir(path, 0700) == 0) {
            return 1;
        } else {
            zox_log(" ! error creating directory");
        }
#endif
    } else {
        zox_log(" > save directory existed [%s]\n", path);
        return 1;
    }
    return 0;
}

byte has_save_game_directory(const char *game) {
    char path[max_path_characters];
    get_save_directory(game, path, sizeof(path));
    // zox_log(" > save path is [%s]\n", path)
    return has_path_directory(path);
}

byte delete_save_directory(const char *game) {
    char path[max_path_characters];
    get_save_directory(game, path, sizeof(path));
    return delete_dir(path);
}