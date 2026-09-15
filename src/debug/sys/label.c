// todo: add raycast position here, and character position

int debug_newline_zext(char buffer[], int buffer_size, int buffer_index) {
    buffer_index += snprintf(buffer + buffer_index, buffer_size, "Day 1.\nToday is a very sunny day.\nHi jerry.");
    return buffer_index;
}

#define get_label_generic_function(Name, name)\
int get_label_##name(ecs *world, char buffer[], int buffer_size, int buffer_index, const entity e) {\
    if (!e || !zox_has(e, Name)) return buffer_index;\
    const Name *children = zox_get(e, Name);\
    buffer_index += snprintf(buffer + buffer_index, buffer_size - buffer_index, "[%s]'s children [%i]\n", zox_get_name(e), children->length);\
    for (int i = 0; i < children->length; i++) {\
        buffer_index += snprintf(buffer + buffer_index, buffer_size - buffer_index, "  [%i] %s\n", i, zox_get_name(children->value[i]));\
    }\
    return buffer_index;\
}

get_label_generic_function(PlayerLinks, player_links)
