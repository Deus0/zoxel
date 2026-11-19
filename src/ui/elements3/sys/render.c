// 3D uis, a canvas at a time: its elements in layer order, painting over each other, depth tested against the world and
//  the canvases drawn before; then its depth written (its solid parts), so later canvases go behind it
//  - canvases aren't sorted (thousands), only each one's elements (under 100): doc/tech/uis.md (the translucency gotcha)
//  - its elements come in tree order (a parent before its children), nearly layer order already: an insertion sort
//  - a hidden element (RenderDisabled) hides its children too, i.e. a scrolled list's rows (texts set their own
//      RenderDisabled when respawned)
#define canvas3_elements_max 512

// its shown elements under e, in tree order
static inline int canvas3_gather(ecs* world, entity e, entity* elements, int count) {
    iter it = zox_children(world, e);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity e2 = it.entities[i];
            if (zox_has(e2, RenderDisabled) && zox_getv(e2, RenderDisabled)) {
                continue;
            }
            if (count < canvas3_elements_max && zox_has(e2, Element3) && zox_has(e2, Layer)) {
                elements[count++] = e2;
            }
            count = canvas3_gather(world, e2, elements, count);
        }
    }
    return count;
}

void canvas3_render_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDisabled);
    const MaterialTextured3D* attributes = NULL;
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDisabled, disabled);
        if (disabled->value) {
            continue;
        }
        entity elements[canvas3_elements_max];
        const int count = canvas3_gather(world, e, elements, 0);
        if (!count) {
            continue;
        }
        for (int a = 1; a < count; a++) {
            const entity element = elements[a];
            const byte layer = zox_getv(element, Layer);
            int b = a;
            while (b > 0 && zox_getv(elements[b - 1], Layer) > layer) {
                elements[b] = elements[b - 1];
                b--;
            }
            elements[b] = element;
        }
        if (!attributes) {
            attributes = element3_draw_begin(world);
            if (!attributes) {
                return;
            }
        }
        float4x4 turn_matrix;
        const float4x4* turn = element3_billboard_turn(world, e, &turn_matrix) ? &turn_matrix : NULL;
        element3_draw_pass(attributes, 0);
        for (int a = 0; a < count; a++) {
            element3_draw(world, elements[a], attributes, turn);
        }
        element3_draw_pass(attributes, 1);
        for (int a = 0; a < count; a++) {
            element3_draw(world, elements[a], attributes, turn);
        }
    }
    if (attributes) {
        element3_draw_end(attributes);
    }
    zox_sys_on_end();
} zoxd_system(canvas3_render_system);
