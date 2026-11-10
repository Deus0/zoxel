// todo: add raycast position here, and character position

int debug_newline_zext(char buffer[], int buffer_size, int buffer_index) {
    buffer_index += snprintf(buffer + buffer_index, buffer_size, "Day 1.\nToday is a very sunny day.\nHi jerry.");
    return buffer_index;
}

// Lists the targets of a links relation, i.e. zoxd_nf_links
#define get_label_links_function(Name, name)\
int get_label_##name(ecs *world, char buffer[], int buffer_size, int buffer_index, const entity e) {\
    if (!e) return buffer_index;\
    buffer_index += snprintf(buffer + buffer_index, buffer_size - buffer_index, "[%s]'s links [%i]\n", zox_get_name(e), zox_get_links_count(world, e, Name));\
    int link_index = 0;\
    zox_each_link(world, e, Name, target) {\
        buffer_index += snprintf(buffer + buffer_index, buffer_size - buffer_index, "  [%i] %s\n", link_index++, zox_get_name(target));\
    }\
    return buffer_index;\
}

get_label_links_function(PlayerLinks, player_links)
