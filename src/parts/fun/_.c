// NOTE: entity_array_d* parts = create_entity_array_d(1);
// NOTE: dispose_entity_array_d(parts);

entity get_item_model(ecs* world, entity e) {
    if (!zox_valid(e)) {
        zox_logw("Item Invalid");
        return 0;
    }
    if (!zox_has(e, ModelLink)) {
        zox_logw("Item has no ModelLink [%s]", zox_get_name(e));
        return 0;
    }
    entity model = zox_getv(e, ModelLink);
    if (!zox_valid(model) || !zox_has(model, ModelLods) || !zox_has(model, MaxRenderDepth)) {
        zox_logw("Item has Invalid Model [%s]", zox_get_name(e));
        return 0;
    }
    byte mdepth = zox_getv(model, MaxRenderDepth);
    zox_geter(model, ModelLods, mlods);
    return mlods->value[mdepth];
}

