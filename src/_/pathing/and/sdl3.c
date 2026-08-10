// sdl3.c
const char* zox_get_android_internal_storage_path() {
    return SDL_GetAndroidInternalStoragePath();
}

JNIEnv* zox_get_android_jni_env() {
    return (JNIEnv*) SDL_GetAndroidJNIEnv();
}

jobject zox_get_android_activity() {
    return (jobject) SDL_GetAndroidActivity();
}