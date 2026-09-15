#define zoxc_lint(T) \
    zoxc(T, lint)

#define zoxd_lint(T)\
    zoxd(T);\
    entity_array_d_add(component_ids_lint, ecs_id(T)); \
    zoxd_reflect_1(T, ecs_i64_t)

#define zox_component_string_lint(component) \
    " li [%ld]", \
    component->value

zox_base_type(lint)
