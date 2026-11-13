// a strip along the bottom of a window's body, its buttons sit on it (i.e. dialogue's Next), the body above it is free
//  for its content (text can't run under its buttons)
entity spawn_footer(ecs* world, entity body, int2 body_size, int height) {
    const int2 size = (int2) { body_size.x, height };
    const entity e = spawn_uic(world, prefab_element_frame, body, (float2) { 0.5f, 0 }, (int2) { 0, height / 2 },
        size, size, footer_fill, footer_outline);
    zox_set_unique_name(e, "footer");
    zox_add(e, WindowFooter);
    return e;
}
