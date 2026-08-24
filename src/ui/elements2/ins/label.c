entity spawn_label(
    ecs *world,
    entity prefab,
    entity parent,
    int2 position,
    float2 anchor,
    byte2 padding,
    const char* text,
    byte font_size,
    byte alignment,
    byte layer,
    color fill,
    color fillo,
    color font_fill,
    color font_fillo)
{
    entity e = spawn_text(
        world,
        prefab,
        parent,
        position,
        anchor,
        font_size,
        alignment,
        padding,
        text,
        font_fill,
        font_fillo);
    zox_name("label");
    zox_setv(e, FillColor, fill);
    zox_setv(e, OutlineColor, fillo);
    zox_setv(e, Layer, layer);
    return e;
}
