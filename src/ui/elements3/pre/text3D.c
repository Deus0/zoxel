typedef struct {
    entity prefab;
    entity parent;
    float3 position;
    byte alignment;         // mesh_alignment
    byte2 padding;          // around zigels
    const char* text;
} Text3DData;

entity spawn_prefab_text3D(
    ecs *world,
    const entity prefab
) {
    zox_prefab_child(prefab);
    zox_prefab_name("text3D");
    zox_add_tag(e, Zext);
    zox_add_tag(e, Text3D);
    zox_prefab_set(e, MeshAlignment, { 0 });
    zox_prefab_set(e, TextFontSize, { 0 });
    zox_prefab_set(e, TextResolution, { 0 });
    zox_prefab_set(e, TextPadding, { byte2_zero });
    zox_prefab_set(e, TextData, { 0 });
    zox_prefab_set(e, TextDirty, { 0 });
    zox_prefab_set(e, ParentLink, { 0 });
    zox_prefab_set(e, FontOutlineColor, { { 255, 0, 0, 255 }});
    zox_prefab_set(e, FontFillColor, { { 0, 255, 0, 255 }});
    zox_prefab_set(e, FontThickness, { 1 });
    zox_prefab_set(e, FontOutlineThickness, { 1 });
    zox_prefab_set(e, Children, { 0 });
    zox_prefab_set(e, RenderDisabled, { 0 });
    zox_prefab_set(e, Text3DScale, { 1 });
    return e;
}