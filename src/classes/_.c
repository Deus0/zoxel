/*
 * +------------------------------------------------------------------+
 * | Zox Module: Classes                                              |
 * |                                                                  |
 * |  Roles - Archetypes - Progression - Abilities - Class State      |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
// zoxc_userdata(Class);
zox_tag(Class);
zoxc_entity(ClassLink);
zoxc_entitiesw(ClassLinks);

void import_classes(ecs* world) {
    zox_module(classes);
    zoxd_tag(Class);
    zoxd_entity(ClassLink);
    zoxd_entitiesw(ClassLinks);
}
