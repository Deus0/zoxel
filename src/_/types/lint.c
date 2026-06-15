#define zoxc_lint(name) \
    zoxc(name, lint)

#define zoxd_lint(name)\
    zoxd(name)\
    entity_array_d_add(component_ids_lint, ecs_id(name));

#define zox_component_string_lint(component) \
    " li [%ld]", component->value

zox_base_type(lint)
