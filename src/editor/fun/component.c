// What the inspector shows for each of an entity's ids (components, tags and pairs)

// the type of target's component, for its row's color and click behaviour
byte fetch_component_type(
    ecs* world,
    entity target,
    entity component)
{
    if (!zox_valid(target)) {
        zox_logw("Invalid target in [fetch_component_type]");
        return zox_type_none;
    }
    if (zox_has_id_flag(component, PAIR)) {
        return zox_type_pair;
    }
    const entity id = zox_id_component(component);
    if (!zox_valid(id)) {
        zox_logw("Invalid component in [fetch_component_type]");
        return zox_type_none;
    }
    if (zox_has_id_flag(id, zox_override_tag)) {
        return zox_type_override;
    }
    #define fetch_component_type_i(T)\
        if (is_component_type_##T(id)) { \
            return zox_type_##T; \
        }
    fetch_component_type_i(byte)
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
    #undef fetch_component_type_i
    // not a zox type yet (i.e. MeshIndicies), or a tag
    return zox_component_size(world, id) ? zox_type_unknown : zox_type_tag;
}

// the label for target's component, i.e. "Position3D [1.0, 2.0, 3.0]"
//  - returns a new string (inspector_component_size_buffer long), the caller frees it
char* fetch_component_label(
    ecs *world,
    entity target,
    entity component)
{
    const uint size = inspector_component_size_buffer;
    char* buffer = malloc(size);
    if (!zox_valid(target)) {
        snprintf(buffer, size, "Invalid Entity");
        return buffer;
    }
    if (zox_has_id_flag(component, PAIR)) {
        const entity first = zox_pair_first(world, component);
        const entity second = zox_pair_second(world, component);
        snprintf(buffer, size, "%s -=- %s",
            zox_valid(first) ? zox_get_name(first) : "Invalid",
            zox_valid(second) ? zox_get_name(second) : "Invalid");
        return buffer;
    }
    const entity id = zox_id_component(component);
    if (!zox_valid(id)) {
        zox_logw("[%s] has an invalid component in [fetch_component_label]", zox_get_name(target));
        snprintf(buffer, size, "Invalid Component");
        return buffer;
    }
    if (zox_has_id_flag(id, zox_override_tag)) {
        snprintf(buffer, size, "*[%s]*", zox_get_name(id));
        return buffer;
    }
    int index = snprintf(buffer, size, "%s", zox_get_name(component));
    #define add_component_label(T)\
        if (is_component_type_##T(id)) { \
            get_type_label_##T(world, target, id, buffer, size, index); \
            return buffer; \
        }
    add_component_label(byte)
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
    #undef add_component_label
    // [T] tag, [?] a component without a zox type
    if (index < (int) size) {
        snprintf(buffer + index, size - index, zox_component_size(world, id) ? " [?]" : " [T]");
    }
    return buffer;
}
