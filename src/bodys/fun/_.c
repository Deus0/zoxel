// NOTE: entity_array_d* parts = create_entity_array_d(1);
// NOTE: dispose_entity_array_d(parts);
void fetch_parts_recursive(ecs *world, entity_array_d* entities, entity e) {
    if (!zox_has(e, PartLinks)) {
        return;
    }
    zox_geter(e, PartLinks, parts);
    for (int i = 0; i < parts->length; i++) {
        entity part = parts->value[i];
        add_to_entity_array_d(entities, part);
        fetch_parts_recursive(world, entities, part);
    }
}

entity get_item_model(ecs* world, entity e) {
    if (!zox_valid(e)) {
        zox_logw("Item Invalid");
        return 0;
    }
    if (!zox_has(e, ModelLink)) {
        zox_logw("Item has no ModelLink [%s]", zox_get_name(e));
        return 0;
    }
    zox_geter_value(e, ModelLink, entity, model);
    if (!zox_valid(model) || !zox_has(model, ModelLods) || !zox_has(model, MaxRenderDepth)) {
        zox_logw("Item has Invalid Model [%s]", zox_get_name(e));
        return 0;
    }
    zox_geter_value(model, MaxRenderDepth, byte, mdepth);
    zox_geter(model, ModelLods, mlods);
    return mlods->value[mdepth];
}
