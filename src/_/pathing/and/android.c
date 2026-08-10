AAssetManager* get_asset_manager() {
    JNIEnv* env = zox_get_android_jni_env();
    if (!env) {
        zox_loge("zox_get_android_jni_env returned NULL");
        return NULL;
    }
    jobject activity = zox_get_android_activity();
    if (!activity) {
        zox_loge("zox_get_android_activity returned NULL");
        return NULL;
    }
    jclass activityClass = (*env)->GetObjectClass(
        env,
        activity);
    if (!activityClass) {
        zox_loge("GetObjectClass failed");
        return NULL;
    }
    jmethodID methodID = (*env)->GetMethodID(
        env,
        activityClass,
        "getAssets",
        "()Landroid/content/res/AssetManager;");
    if (!methodID) {
        zox_loge("GetMethodID failed for getAssets()");
        return NULL;
    }
    jobject assetManagerObj = (*env)->CallObjectMethod(
        env,
        activity,
        methodID);
    if (!assetManagerObj) {
        zox_loge("CallObjectMethod returned NULL for getAssets()");
        return NULL;
    }
    AAssetManager* manager = AAssetManager_fromJava(env, assetManagerObj);
    if (!manager) {
        zox_loge("AAssetManager_fromJava() failed");
        return NULL;
    }
    return manager;
}

// remember - source is assets, the jar apk
void extract_android_assets(
    AAssetManager* asset_manager,
    const char* source_path,
    const char* destination_path)
{
    zox_logv("📦 Decompressing [%s] -> [%s]", source_path, destination_path);
    AAssetDir* asset_dir = AAssetManager_openDir(asset_manager, source_path);
    if (!asset_dir) {
        zox_loge("asset directory does not exist [%s]", source_path);
        return;
    }
    if (!android_create_directory_r(destination_path)) {
        zox_loge("Failed Destination Directory [%s]", destination_path);
        return;
    }
    zox_logv("Success creating directory [%s]", destination_path);
    const char* filename = NULL;
    while ((filename = AAssetDir_getNextFileName(asset_dir)) != NULL) {
        // sanity checks for our slashes
        char nested_source_path[1024];
        char nested_dest_path[1024];
        snprintf(nested_source_path, sizeof(nested_source_path), "%s%s%s",
            source_path,
            (source_path[strlen(source_path) - 1] == '/') ? "" : "/",
            filename);
        snprintf(nested_dest_path, sizeof(nested_dest_path), "%s%s%s",
            destination_path,
            (destination_path[strlen(destination_path) - 1] == '/') ? "" : "/",
            filename);
        AAsset* asset = AAssetManager_open(
            asset_manager,
            nested_source_path,
            AASSET_MODE_BUFFER);
        if (asset) {
            const void* data = AAsset_getBuffer(asset);
            size_t size = AAsset_getLength(asset);
            FILE* out = fopen(nested_dest_path, "wb");
            if (data && out && size) {
                zox_logv("+ opened -> [%s] s[%i]", nested_source_path, (int) size);
                fwrite(data, size, 1, out);
                fclose(out);
            } else {
                zox_loge("⚠️ failed writing to [%s]", nested_dest_path);
            }
            AAsset_close(asset);
        } else {
            zox_loge("asset failed to open [%s]", nested_source_path);
        }
    }
    AAssetDir_close(asset_dir);
}

void android_assets_init(AAssetManager *assetManager, char *path) {
    char path_input[1024];
    char path_output[1024];
    snprintf(path_input, 1024, "%s%s", resources_folder_name, path);
    snprintf(path_output, 1024, "%s%s", resources_path, path);
    extract_android_assets(assetManager, path_input, path_output);
}

// todo: auto generate assets.txt later from zoxelder
void decompress_android_resources(const char* resources_path) {
    if (!resources_path) {
        zox_loge("[decompress_android_resources]: resources_path is null.");
        return;
    }
    zox_logv("Initial Decompression at [%s]", resources_path);
    AAssetManager *manager = get_asset_manager();
    if (!manager) {
        zox_loge("[decompress_android_resources]: Android AssetManager Null");
        return;
    }
    zox_logv("Deleting old Resources [%s]", resources_path);
    delete_directory_recursive(resources_path);
    zox_logv("Creating Export Path [%s]", resources_path);
    if (!android_create_directory_r(resources_path)) {
        zox_loge("[decompress_android_resources]: could not create directory: %s", resources_path);
        return;
    }
    zox_logv("Created new directory [%s]", resources_path);
    // android directories
    // NOTE: assets.txt is a list of folders
    char assets_txt_filepath[1024];
    snprintf(assets_txt_filepath, 1024, "%sassets.txt", resources_folder_name);
    int dir_len = 0;
    char** dirs = get_assets_dirs(manager, assets_txt_filepath, &dir_len);
    if (dirs) {
        zox_logv("Directories Found [%i]", dir_len);
        for (int i = 0; i < dir_len; i++) {
            zox_logv("  - [%i] [%s]", i, dirs[i]);
            android_assets_init(manager, dirs[i]);
            free(dirs[i]); // free each strdup'd string after usage
        }
        free(dirs);
    } else {
        zox_loge("Failed to load asset directories from assets.txt");
    }
}

byte initialize_pathing_android() {
    const char* base_path = zox_get_android_internal_storage_path();
    if (!base_path) {
        zox_loge("[pathing_android] failed to get base_path");
        return EXIT_FAILURE;
    }
    zox_logv("Android Base Path [%s]", base_path);
    data_path = clone_str(base_path);
    DIR* dir = opendir(base_path);
    if (dir) {
        resources_path = malloc(strlen(base_path) + strlen("/"resources_dir_name"/") + 1);
        strcpy(resources_path, base_path);
        strcat(resources_path, "/"resources_dir_name"/");
        zox_logv("resources_path [%s]", resources_path);
        closedir(dir);
    } else if (ENOENT == errno) {
        zox_loge("SDL data_path (DOES NOT EXIST): %s", data_path);
    } else {
        zox_loge("SDL data_path (MYSTERIOUSLY DOES NOT EXIST): %s", data_path);
    }
    return EXIT_SUCCESS;
}