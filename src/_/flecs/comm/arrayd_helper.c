// equals function required here
#define zoxc_arrayd_removes(T, type) \
    byte remove_from_##T(T* ptr, const type data) { \
        spin_lock(&ptr->lock); \
        int index = -1; \
        for (int i = 0; i < ptr->length; i++) { \
            if (ptr->value[i] == data) { \
                index = i; \
                break;\
            }\
        }\
        spin_unlock(&ptr->lock); \
        if (index == -1) { \
            return 0; \
        } \
        return remove_at_##T(ptr, index);\
    }

#define resize_memory_component(T, ptr, type, new_length) {\
    if (ptr->length != new_length) {\
        if (new_length == 0) {\
            dispose_##T(ptr);\
        } else if (!ptr->value) {\
            initialize_##T(ptr, new_length);\
        } else {\
            type* new_memory = rezalloc(ptr->value, new_length * sizeof(type));\
            if (!new_memory) {\
                zox_log_error("Failure with realloc");\
            } else {\
                ptr->value = new_memory;\
                ptr->length = new_length;\
            }\
        }\
    }\
}
