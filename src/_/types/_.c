#include "types.c"
#include "keys.c"
#include "zext.c"
#include "byte.c"
#include "byte2.c"
#include "byte3.c"
#include "int.c"
#include "int2.c"
#include "int3.c"
#include "int4.c"
#include "uint.c"
#include "uint2.c"
#include "guint.c"
#include "guint2.c"
#include "lint.c"
#include "float.c"
#include "float2.c"
#include "float3.c"
#include "float4.c"
#include "float6.c"
#include "double.c"
#include "color.c"
#include "color_rgb.c"
#include "entity.c"
#include "entity2.c"
#include "entity3.c"
#include "entities.c"
#include "ushort.c"
#include "ulong.c"

#define zox_component_string(type) zox_component_string##_##type(component##_##type)

// Dynamic Array Definitions
zoxel_dynamic_array(byte)
zoxel_dynamic_array(int)
create_array_d_has(int)
zoxel_dynamic_array(int3)
zoxel_dynamic_array(float2)
zoxel_dynamic_array(float3)
zoxel_dynamic_array(float4)
zoxel_dynamic_array(float4x4)

void initialize_component_ids() {
    initialize_component_ids_text();
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
    initialize_component_ids_uint2();
    initialize_component_ids_guint();
    initialize_component_ids_guint2();
    initialize_component_ids_float();
    initialize_component_ids_float2();
    initialize_component_ids_float3();
    initialize_component_ids_float4();
    initialize_component_ids_float6();
    initialize_component_ids_entity();
    initialize_component_ids_entity2();
    initialize_component_ids_entity3();
    initialize_component_ids_entities();
    initialize_component_ids_color();
    initialize_component_ids_color_rgb();
    initialize_component_ids_ulong();
    initialize_component_ids_ushort();
}

void dispose_component_ids() {
    dispose_component_ids_text();
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
    dispose_component_ids_uint2();
    dispose_component_ids_guint();
    dispose_component_ids_guint2();
    dispose_component_ids_float();
    dispose_component_ids_float2();
    dispose_component_ids_float3();
    dispose_component_ids_float4();
    dispose_component_ids_float6();
    dispose_component_ids_entity();
    dispose_component_ids_entity2();
    dispose_component_ids_entity3();
    dispose_component_ids_entities();
    dispose_component_ids_color();
    dispose_component_ids_color_rgb();
    dispose_component_ids_ulong();
    dispose_component_ids_ushort();
}
