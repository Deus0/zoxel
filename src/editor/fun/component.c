byte fetch_component_type(ecs* world, entity target, entity component) {
    if (!zox_valid(target)) {
        zox_logw("Invalid target in [fetch_component_type]");
        return zox_type_none;
    }
    if (zox_has_id_flag(component, PAIR)) {
        return zox_type_pair;
    }
    ecs_id_t id = component & ECS_COMPONENT_MASK;
    if (!zox_valid(id)) {
        zox_logw("invalid component in [fetch_component_type]");
        return zox_type_none;
    }
    if (zox_has_id_flag(id, zox_override_tag)) {
        return zox_type_override;
    }
    #define fetch_component_type_i(T)\
        else if (is_component_type_##T(id)) { \
            return zox_type_##T; \
        }

    if (is_component_type_byte(id)) {
        return zox_type_byte;
    }
    fetch_component_type_i(byte2)
    fetch_component_type_i(byte3)
    fetch_component_type_i(int)
    fetch_component_type_i(int2)
    fetch_component_type_i(int3)
    fetch_component_type_i(int4)
    fetch_component_type_i(uint)
    fetch_component_type_i(uint2)
    fetch_component_type_i(float)
    fetch_component_type_i(float2)
    fetch_component_type_i(float3)
    fetch_component_type_i(float4)
    fetch_component_type_i(float6)
    fetch_component_type_i(double)
    fetch_component_type_i(lint)
    fetch_component_type_i(entity)
    fetch_component_type_i(entity2)
    fetch_component_type_i(entity3)
    fetch_component_type_i(entities)
    fetch_component_type_i(color)
    fetch_component_type_i(color_rgb)
    fetch_component_type_i(text)
    fetch_component_type_i(ulong)
    fetch_component_type_i(gint)
    fetch_component_type_i(guint)
    fetch_component_type_i(guint2)
    else {
        const EcsComponent* component = (EcsComponent*) ecs_get(world, id, EcsComponent);
        uint component_size = component != NULL ? component->size : 0;
        if (!component_size) {
            return zox_type_tag;
        } else {
            // havn't added yet to zox: MeshIndicies etc
            return zox_type_unknown;
        }
    }
    return zox_type_none;
}

char* fetch_compoent_label(ecs *world, entity target, entity component) {
    uint tlength = inspector_component_size_buffer;
    char* buffer = malloc(tlength);
    if (!zox_valid(target)) {
        // zox_logw("Invalid target [Entity] in [fetch_compoent_label]");
        snprintf(buffer, tlength, "Invalid Entity");
        return buffer;
    }
    if (zox_has_id_flag(component, PAIR)) {
        entity pair1 = ecs_pair_first(world, component);
        entity pair2 = ecs_pair_second(world, component);
        if (!zox_valid(pair1) && !zox_valid(pair2)) {
            snprintf(buffer, tlength, "Invalid Pair");
        } else if (!zox_valid(pair1)) {
            snprintf(buffer, tlength, "Invalid -=- %s", zox_get_name(pair2));
        } else if (!zox_valid(pair2)) {
            snprintf(buffer, tlength, "%s -=- Invalid", zox_get_name(pair1));
        } else {
            snprintf(buffer, tlength, "%s -=- %s", zox_get_name(pair1), zox_get_name(pair2));
        }

        return buffer;
    }
    ecs_id_t id = component & ECS_COMPONENT_MASK;
    if (!zox_valid(id)) {
        zox_logw("%s has Invalid [Component] in [fetch_compoent_label]", zox_get_name(target));
        snprintf(buffer, tlength, "Invalid Component");
        return buffer;
    }
    if (zox_has_id_flag(id, zox_override_tag)) {
        snprintf(buffer, tlength, "*[%s]*", zox_get_name(id));
        return buffer;
    }
    int index = 0;
    index += snprintf(buffer + index, tlength, "%s", zox_get_name(component));

    #define add_component_label(T)\
        else if (is_component_type_##T(id)) { \
            index = get_type_label_##T(world, target, id, buffer, tlength, index); \
        }

    // TODO: Make this dynamically use all types
    if (is_component_type_byte(id)) {
        index = get_type_label_byte(world, target, id, buffer, tlength, index);
    }
    add_component_label(byte2)
    add_component_label(byte3)
    add_component_label(int)
    add_component_label(int2)
    add_component_label(int3)
    add_component_label(int4)
    add_component_label(uint)
    add_component_label(uint2)
    add_component_label(float)
    add_component_label(float2)
    add_component_label(float3)
    add_component_label(float4)
    add_component_label(float6)
    add_component_label(double)
    add_component_label(lint)
    add_component_label(entity)
    add_component_label(entity2)
    add_component_label(entity3)
    add_component_label(entities)
    add_component_label(color)
    add_component_label(color_rgb)
    add_component_label(text)
    add_component_label(gint)
    add_component_label(guint)
    add_component_label(guint2)
    else {
        const EcsComponent* component = (EcsComponent*) ecs_get(world, id, EcsComponent);
        uint component_size = component !=  NULL ? component->size : 0;
        if (!component_size) {
            index += snprintf(buffer + index, tlength - index, " [T]");
        } else {
            index += snprintf(buffer + index, tlength - index, " [?]");
        }
    }
    return buffer;
}
