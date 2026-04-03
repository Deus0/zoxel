const byte zox_types_length = 32;
const byte zox_type_labels_length = 32;

#define zox_type_none 0
#define zox_type_tag 1
#define zox_type_pair 2
#define zox_type_override 3
#define zox_type_name 4
#define zox_type_text 5
#define zox_type_unknown 6

#define zox_type_byte 7
#define zox_type_byte2 8
#define zox_type_byte3 9

#define zox_type_int 10
#define zox_type_int2 11
#define zox_type_int3 12
#define zox_type_int4 13

#define zox_type_float 14
#define zox_type_float2 15
#define zox_type_float3 16
#define zox_type_float4 17
#define zox_type_float6 18

#define zox_type_double 19
#define zox_type_double2 20
#define zox_type_double3 21

#define zox_type_uint 22
#define zox_type_uint2 23

#define zox_type_lint 24

#define zox_type_color 25
#define zox_type_color_rgb 26

#define zox_type_entity 27
#define zox_type_entity2 28
#define zox_type_entity3 29
#define zox_type_entities 30

#define zox_type_ulong 31

const char* zox_type_labels[] = {
    "none",
    "tag",
    "pair",
    "override",
    "name",
    "text",
    "unknown",

    "byte",
    "byte2",
    "byte3",

    "int",
    "int2",
    "int3",
    "int4",

    "float",
    "float2",
    "float3",
    "float4",
    "float6",

    "double",
    "double2",
    "double3",

    "uint",
    "uint2",

    "lint",
    "color",
    "color_rgb",

    "entity",
    "entity2",
    "entity3",
    "entities",

    "ulong",
};
