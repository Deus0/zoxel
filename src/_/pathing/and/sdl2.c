// sdl2.c
const char* zox_get_android_internal_storage_path() {
    return SDL_AndroidGetInternalStoragePath();
}

JNIEnv* zox_get_android_jni_env() {
    return (JNIEnv*) SDL_AndroidGetJNIEnv();
}

jobject zox_get_android_activity() {
    return (jobject) SDL_AndroidGetActivity();
}