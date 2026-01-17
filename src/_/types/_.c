#include "byte.c"
#include "byte2.c"
#include "byte3.c"
#include "lint.c"
#include "double.c"
#include "int.c"
#include "int2.c"
#include "int3.c"
#include "int4.c"
#include "uint.c"
#include "float.c"
#include "float2.c"
#include "float3.c"
#include "float4.c"
#include "float6.c"
#include "entity.c"
#include "entity2.c"
#include "entity3.c"
#include "color.c"
#include "color_rgb.c"
#include "zext.c"

#define zox_component_string(type) zox_component_string##_##type(component##_##type)

void initialize_component_ids() {
    initialize_component_ids_byte();
    initialize_component_ids_byte2();
    initialize_component_ids_byte3();
    initialize_component_ids_lint();
    initialize_component_ids_double();
    initialize_component_ids_int();
    initialize_component_ids_int2();
    initialize_component_ids_int3();
    initialize_component_ids_int4();
    initialize_component_ids_uint();
    initialize_component_ids_float();
    initialize_component_ids_float2();
    initialize_component_ids_float3();
    initialize_component_ids_float4();
    initialize_component_ids_float6();
    initialize_component_ids_entity();
    initialize_component_ids_color();
    initialize_component_ids_color_rgb();
    initialize_component_ids_text();
}

void dispose_component_ids() {
    dispose_component_ids_byte();
    dispose_component_ids_byte2();
    dispose_component_ids_byte3();
    dispose_component_ids_lint();
    dispose_component_ids_double();
    dispose_component_ids_int();
    dispose_component_ids_int2();
    dispose_component_ids_int3();
    dispose_component_ids_int4();
    dispose_component_ids_uint();
    dispose_component_ids_float();
    dispose_component_ids_float2();
    dispose_component_ids_float3();
    dispose_component_ids_float4();
    dispose_component_ids_float6();
    dispose_component_ids_entity();
    dispose_component_ids_color();
    dispose_component_ids_color_rgb();
    dispose_component_ids_text();
}

// identitys
zoxc_int(ID);
zoxc_lint(Seed);
// zoxc_text(ZoxName)
zoxc_fixed_string(ZoxName, 64);
// time events
zox_tag(DestroyInFrame);
zoxc_double(EventTime);
zoxc_entity(EventInput);
zoxc_function(TimedEvent, void, ecs*, const entity);
// used for editor
zoxc_byte(EntityDirty);
zoxc_entity(EntityTarget);
zoxc_entity(ComponentTarget);
// unsorted
zoxc_byte(GenericEvent);
zoxc_state(EntityInitialize);
zoxc_state(Generate);
zoxc_byte(SpawnChance);
zoxc_fixed_string(SaveGamePath, 512);

void define_components_generic(ecs* world) {
    // ids
    zoxd_int(ID);
    zoxd_lint(Seed);
    // zoxd_text(ZoxName);
    zoxd_fixed_string(ZoxName);
    // timed
    zoxd_tag(DestroyInFrame);
    zoxd_double(EventTime);
    zoxd_entity(EventInput);
    zoxd(TimedEvent);
    // editor
    zoxd_byte(EntityDirty);
    zoxd_entity(EntityTarget);
    zoxd_entity(ComponentTarget);
    // unsorted
    zoxd_byte(GenericEvent);
    zoxd_state(EntityInitialize);
    zoxd_byte(SpawnChance);
    zoxd_state(Generate);
    zoxd_fixed_string(SaveGamePath);
}