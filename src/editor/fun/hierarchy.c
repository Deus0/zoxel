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

void zox_print_entity(ecs *world, entity e) {
    const ecs_type_t *type = ecs_get_type(world, e);
    const ecs_id_t *type_ids = type->array;
    int32_t i, count = type->count;
    char *type_str = ecs_type_str(world, type);
    zox_log("       > ecs_types: %s\n", type_str)
    free(type_str);
    for (i = 0; i < count; i ++) {
        ecs_id_t id = type_ids[i];
        zox_log("       > ")
        if (ECS_HAS_ID_FLAG(id, PAIR)) {
            entity rel = ecs_pair_first(world, id);
            entity tgt = ecs_pair_second(world, id);
            zox_log(" pair %s [%lu]", ecs_get_name(world, rel), tgt)
        } else {
            entity comp = id & ECS_COMPONENT_MASK;
            zox_log(" %lu", comp)
            zox_log(" %s", ecs_get_name(world, comp))
            // how to print void*, get*/ base type of ->value of component
            // zox_log(" [%i]", ecs_get_id(world, target, id)->value)
            if (is_component_type_byte(id)) {
                const EntityDirty *byte_component = ((const EntityDirty*) ecs_get_id(world, e, id));
                if (byte_component) {
                    zox_log(" b [%i]", byte_component->value)
                }
            } else if (is_component_type_lint(id)) {
                const Seed *c = ((const Seed*) ecs_get_id(world, e, id));
                if (c) {
                    zox_log(" lu [%lu]", c->value)
                }
            } else if (is_component_type_int2(id)) {
                const DraggingDelta *int2_component = ((const DraggingDelta*) ecs_get_id(world, e, id));
                if (int2_component) {
                    zox_log(" i2 [%ix%i]", int2_component->value.x, int2_component->value.y)
                }
            } else if (is_component_type_int(id)) {
                const ID *int_component = ((const ID*) ecs_get_id(world, e, id));
                if (int_component) {
                    zox_log(" i [%i]", int_component->value)
                }
            } else if (is_component_type_float(id)) {
                const Brightness *component_float = ((const Brightness*) ecs_get_id(world, e, id));
                if (component_float) {
                    zox_log(" f [%f]", component_float->value);
                }
            } else if (is_component_type_color(id)) {
                const Color *c = ((const Color*) ecs_get_id(world, e, id));
                if (c) {
                    zox_log(" c [%i.%i.%i]", c->value.r, c->value.g, c->value.b);
                }
            } else {
                const ZoxName *name = ((const ZoxName*) ecs_get_id(world, e, id));
                zox_log(" %i: [%s]", strlen(name->value), name->value);
                //const EntityDirty *byte_component = ((const EntityDirty*) ecs_get_id(world, target, id));
                //if (byte_component) zox_log(" [uknown]")
                // convert_zext_to_text(zoxName->value, zoxName->length))
                // else zox_log(" [tag]")
            }
            /*const long int *value = ((const long int*) ecs_get_id(world, target, id));
            if (value) zox_log(" [%lu]", value)*/
        }
        zox_log("\n")
    }
}

/*void editor_select_entity(ecs *world, entity player, entity e) {

    if (editor_selected == e) {
        return;
    }

    editor_selected = e;

    zox_geter_value(player, CanvasLink, entity, canvas);
    entity inspector = get_canvas_window(world, canvas, zox_window_inspector);

    set_inspector_element(world, inspector, e);
}*/

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
