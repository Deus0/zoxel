// note: not sure why this breaks the first time, maybe flecs table issues?
#ifdef zox_glitch_fix_hierarchy_labels
const byte max_hierarchy_labels = 23;
unsigned is_first_hierarchy_spawn = 1;
#endif
const int hierarchy_max_line_characters = 64;
entity editor_selected;
extern void add_to_labels_voxel_links(ecs *world, entity e, text_group_dynamic_array_d* labels, entity_array_d* entities, int tree_level);
extern void add_to_labels_stat_links(ecs *world, entity e, text_group_dynamic_array_d* labels, entity_array_d* entities, int tree_level);
extern entity prefab_app;
extern entity prefab_window;
extern entity prefab_button;
extern entity prefab_zext;
extern entity prefab_zigel;
extern entity prefab_realm;
extern entity prefab_block;
extern entity prefab_texture;

void add_entity_to_labels(ecs *world, entity e, text_group_dynamic_array_d* labels, entity_array_d* entities, int tree_level) {

    if (!zox_valid(e)) {
        return;
    }

    char *text = malloc(hierarchy_max_line_characters);
    if (!zox_has(e, ZoxName)) {
        snprintf(text, hierarchy_max_line_characters, "%s", zox_get_name(e));
    } else {
        zox_geter(e, ZoxName, zox_name);
        snprintf(text, hierarchy_max_line_characters, "%s", zox_name->value);
        /*char *text2 = convert_zext_to_text(zox_name->value, zox_name->length);
        if (text2) {
            snprintf(text, hierarchy_max_line_characters, "%s", text2);
            free(text2);
        } else {
            snprintf(text, hierarchy_max_line_characters, "[%s]", zox_get_name(e));
        }*/
    }

    for (int i = 0; i < tree_level; i++) {
        char *temp = strdup(text);
        if (temp) {
            snprintf(text, hierarchy_max_line_characters, ".%s", temp);
            free(temp);
        } else {
            zox_log_error("no temp was created")
        }
    }
    // zox_log("%s made label [%s]", zox_get_name(e), text)
    add_to_text_group_dynamic_array_d(labels, (text_group_dynamic) { text = text });
    add_to_entity_array_d(entities, e);
}

int get_max_characters_d(const char *header_label, text_group_dynamic_array_d* labels) {

    int max_characters = 0; // get max text length out of all of the words
    for (size_t i = 0; i < labels->size; i++) {
        int txt_size = strlen(labels->data[i].text);
        if (txt_size > max_characters) {
            max_characters = txt_size;
        }
    }

    int header_txt_size = strlen(header_label);
    if (header_txt_size > max_characters) {
        max_characters = header_txt_size;
    }

    return max_characters;
}

void add_entity_children_to_labels(ecs *world, entity e, text_group_dynamic_array_d* labels, entity_array_d* entities, int tree_level) {

    if (!zox_valid(e)) {
        return;
    }

    add_entity_to_labels(world, e, labels, entities, tree_level);
    if (zox_has(e, Children)) {
        tree_level++;
        zox_geter(e, Children, children)
        for (int i = 0; i < children->length; i++) {
            add_entity_children_to_labels(world, children->value[i], labels, entities, tree_level);
        }
    }
}

void fetch_entity_list_by_id(ecs *world, entity e, entity id, text_group_dynamic_array_d* labels, entity_array_d* entities, int tree_level) {

    if (!zox_valid(e)) {
        return;
    }

    if (!zox_has_id(e, id)) {
        return;
    }

    tree_level++;

    // Transforms our void* to Children*
    const Children* children = (Children*) (zox_get_id(e, id));

    for (int i = 0; i < children->length; i++) {
        entity child = children->value[i];

        if (!zox_valid(child)) {
            continue;
        }

        add_entity_to_labels(world, child, labels, entities, tree_level);

        fetch_entity_list_by_id(world, child, id, labels, entities, tree_level);
    }
}
