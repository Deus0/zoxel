/*#define zox_set_zext_component(e, name, text) \
    zox_set(e, name, { strlen(text), convert_string_to_zext(text) });*/

// #define zox_debug_spawning
// extern void set_new_zox_name(ecs *world, const entity e, const char* text);

/*char *lint_itoa(char *str, long int num) {
    if (str == NULL) return NULL;
    sprintf(str, "%lu", num);
    return str;
}

#define e_string_length 128*/

/*char* get_entity_string(const entity e, const char* name) {
    if (!e || !name) {
        return NULL;
    }
    static char e_string[e_string_length];
    lint_itoa(e_string, (long int) e); // (long int) (rand() % 100000));   //
    int entity_name_length = strlen(name) + 1 + e_string_length + 1;
    char* entity_name = malloc(entity_name_length);
    for (int i = 0; i < entity_name_length; i++) {
        entity_name[i] = 0;
    }
    strcat(entity_name, name);
    strcat(entity_name, "_");
    strcat(entity_name, e_string);
    return entity_name;
}*/

/*char* get_entity_string(const entity e, const char* name) {
    if (!e || !name) return NULL;

    static char e_string[e_string_length]; // example safe size
    lint_itoa(e_string, (long int)e);
    size_t elen = strlen(e_string);
    size_t nlen = strlen(name);
    size_t total_len = nlen + 1 + elen + 1;

    char* entity_name = malloc(total_len);
    if (!entity_name) return NULL;

    memcpy(entity_name, name, nlen);
    entity_name[nlen] = '_';
    memcpy(entity_name + nlen + 1, e_string, elen);
    entity_name[total_len - 1] = '\0';

    zox_log("+ new flecs name [%s]", entity_name);
    return entity_name;
}



void zox_set_entity_name(ecs *world, const entity e, const char* name) {
    if (!e) {
        return;
    }
    char* name_plus_id = get_entity_string(e, name);
    if (name_plus_id) {
        zox_set_name(e, name_plus_id);
    }
    // zox_set_zext_component(e, ZoxName, name)
    // set_new_zox_name(world, e, name);
    // zox_set(e, ZoxName, { strlen(name), convert_string_to_zext(name) })
#ifdef zox_debug_spawning
    zox_log(" + spawned %s [%lu] [%s]", name, e, name_plus_id);
#endif
    free(name_plus_id);
}*/

/*void get_entity_string_stack(
    char* buffer,
    size_t buffer_size,
    entity e,
    const char* name
) {
    if (!e || !name || buffer_size == 0) {
        if (buffer_size > 0) buffer[0] = '\0';
        return;
    }
    static char e_string[32]; // safe size for entity id string
    lint_itoa(e_string, (long int)e);

    size_t nlen = strlen(name);
    size_t elen = strlen(e_string);
    size_t needed = nlen + 1 + elen + 1; // name + '_' + id + null

    if (buffer_size < needed) {
        // truncate safely or just empty string
        buffer[0] = '\0';
        return;
    }

    memcpy(buffer, name, nlen);
    buffer[nlen] = '_';
    memcpy(buffer + nlen + 1, e_string, elen);
    buffer[needed - 1] = '\0';
}
*/
