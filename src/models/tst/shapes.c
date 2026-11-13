// tst: model_shapes, the realm's generated models have the shapes they're meant to (their voxels, not the look)
//  - every model: something filled, in one piece (6 neighbour flood fill: a chest's belly apart from its ribs fails)
//  - body parts mirror left to right (solid or not, colours may differ: eyes)
//  - Chest: no waist pinch, no layer much smaller than the layers above and below it (the belly joined to the ribs)
//  - Head: a neck at the bottom, centred, narrower than the head, the widest above it; eyes showing on the face
//  - cokie: chips in it (both shades), standing out from the dough (HueComplement), and crumbs on it
//  - logs each model's size and pieces; run: bash bsh/tst.sh model_shapes (models to look at: bash bsh/model.sh <name>)
#define zox_tst_shapes_max 32
byte zox_tst_shapes_stage = 0;
int zox_tst_shapes_frames = 0;
entity zox_tst_shapes_realm = 0;
const char* zox_tst_shapes_needed[] = { "Head", "Chest", "cokie" };
#define zox_tst_shapes_needed_count 3
// mirrored left to right (the realm's body parts that are symmetric on their own, not a left or right limb's)
const char* zox_tst_shapes_mirrored[] = { "Head", "Chest", "Hips", "Head2", "Chest2" };
#define zox_tst_shapes_mirrored_count 5

// a voxel grid read from a model's octree, for the shape checks
typedef struct {
    int length;
    byte depth;
    byte* voxels;
} zox_tst_grid;

static inline byte zox_tst_grid_get(const zox_tst_grid* g, int x, int y, int z) {
    if (x < 0 || y < 0 || z < 0 || x >= g->length || y >= g->length || z >= g->length) {
        return 0;
    }
    return g->voxels[x + g->length * (y + g->length * z)];
}

zox_tst_grid zox_tst_grid_read(ecs* world, entity vox) {
    zox_tst_grid g = { 0, 0, NULL };
    if (!zox_valid(vox) || !zox_has(vox, VoxelNode) || !zox_has(vox, NodeDepth)) {
        return g;
    }
    g.depth = zox_getv(vox, NodeDepth);
    g.length = octree_size(g.depth);
    g.voxels = calloc((size_t) g.length * g.length * g.length, 1);
    const VoxelNode* tree = zox_get(vox, VoxelNode);
    for (int x = 0; x < g.length; x++)
    for (int y = 0; y < g.length; y++)
    for (int z = 0; z < g.length; z++) {
        g.voxels[x + g.length * (y + g.length * z)] = getv_VoxelNode((VoxelNode*) tree, g.depth, (byte3) { x, y, z });
    }
    return g;
}

// solid voxels, and how many pieces they're in (6 neighbours)
void zox_tst_grid_pieces(const zox_tst_grid* g, int* solid, int* pieces, int* biggest) {
    const int volume = g->length * g->length * g->length;
    byte* seen = calloc(volume, 1);
    int* stack = malloc(sizeof(int) * volume);
    *solid = 0;
    *pieces = 0;
    *biggest = 0;
    for (int i = 0; i < volume; i++) {
        if (!g->voxels[i]) {
            continue;
        }
        (*solid)++;
        if (seen[i]) {
            continue;
        }
        (*pieces)++;
        int size = 0;
        int top = 0;
        stack[top++] = i;
        seen[i] = 1;
        while (top) {
            const int at = stack[--top];
            size++;
            const int x = at % g->length, y = (at / g->length) % g->length, z = at / (g->length * g->length);
            const int3 sides[6] = { { 1, 0, 0 }, { -1, 0, 0 }, { 0, 1, 0 }, { 0, -1, 0 }, { 0, 0, 1 }, { 0, 0, -1 } };
            for (int s = 0; s < 6; s++) {
                const int nx = x + sides[s].x, ny = y + sides[s].y, nz = z + sides[s].z;
                if (!zox_tst_grid_get(g, nx, ny, nz)) {
                    continue;
                }
                const int n = nx + g->length * (ny + g->length * nz);
                if (!seen[n]) {
                    seen[n] = 1;
                    stack[top++] = n;
                }
            }
        }
        if (size > *biggest) {
            *biggest = size;
        }
    }
    free(seen);
    free(stack);
}

// solid voxels whose mirror across the solid's middle in x isn't solid
int zox_tst_grid_asymmetry(const zox_tst_grid* g) {
    int lower = g->length, upper = -1;
    for (int x = 0; x < g->length; x++)
    for (int y = 0; y < g->length; y++)
    for (int z = 0; z < g->length; z++) {
        if (zox_tst_grid_get(g, x, y, z)) {
            lower = int_min(lower, x);
            upper = int_max(upper, x);
        }
    }
    int differ = 0;
    for (int x = lower; x <= upper; x++)
    for (int y = 0; y < g->length; y++)
    for (int z = 0; z < g->length; z++) {
        differ += (zox_tst_grid_get(g, x, y, z) != 0) != (zox_tst_grid_get(g, lower + upper - x, y, z) != 0);
    }
    return differ;
}

// a layer's solid extent in x (y is up), -1 when empty
void zox_tst_grid_layer(const zox_tst_grid* g, int y, int* lower, int* upper) {
    *lower = g->length;
    *upper = -1;
    for (int x = 0; x < g->length; x++)
    for (int z = 0; z < g->length; z++) {
        if (zox_tst_grid_get(g, x, y, z)) {
            *lower = int_min(*lower, x);
            *upper = int_max(*upper, x);
        }
    }
}

// a realm child named base (its name, or base_<id>): Head, not Head2
byte zox_tst_shapes_named(const char* name, const char* base) {
    const size_t length = strlen(base);
    if (!name || strncmp(name, base, length) != 0) {
        return 0;
    }
    return name[length] == 0 || name[length] == '_';
}

entity zox_tst_shapes_find(ecs* world, entity realm, const char* base) {
    iter it = zox_children(world, realm);
    while (zox_children_next(it)) {
        for (int j = 0; j < it.count; j++) {
            const entity child = it.entities[j];
            if (zox_tst_shapes_named(zox_getn(child), base) && zox_tst_model_views_mesh_of(world, child)) {
                zox_children_end(it);
                return child;
            }
        }
    }
    return 0;
}

byte zox_tst_model_shapes_ready(ecs* world) {
    zox_tst_shapes_frames++;
    zox_tst_progress(zox_tst_shapes_stage * 10000 + zox_tst_shapes_frames);
    if (zox_tst_shapes_stage == 0) {
        zox_tst_waiting = "the game and app to spawn a realm into";
        if (!zox_valid(local_game) || !zox_valid(main_app) || zox_tst_shapes_frames < 5) {
            return 0;
        }
        zox_tst_shapes_realm = spawn_realm(world, local_game, 666);
        zox_setv(zox_tst_shapes_realm, GenerateRealm, zox_generate_realm_start);
        zox_tst_shapes_stage = 1;
        zox_tst_shapes_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the realm to generate its models (Head, Chest, cokie)";
    if (zox_getv(zox_tst_shapes_realm, GenerateRealm)) {
        return 0;
    }
    for (int i = 0; i < zox_tst_shapes_needed_count; i++) {
        if (!zox_tst_shapes_find(world, zox_tst_shapes_realm, zox_tst_shapes_needed[i])) {
            return 0;
        }
    }
    // the rest of the models a little longer
    return zox_tst_shapes_frames > 30;
}

void zox_tst_model_shapes_check(ecs* world) {
    const entity realm = zox_tst_shapes_realm;
    int checked = 0;
    iter it = zox_children(world, realm);
    while (zox_children_next(it)) {
        for (int j = 0; j < it.count; j++) {
            const entity child = it.entities[j];
            if (!zox_tst_model_views_mesh_of(world, child)) {
                continue;
            }
            const entity model = zox_has(child, ModelLink) ? zox_get_link(world, child, ModelLink) : child;
            const entity vox = get_model_vox(world, model);
            zox_tst_grid g = zox_tst_grid_read(world, vox);
            if (!g.voxels) {
                continue;
            }
            // vox files (unnamed, made by hand: a hat's feather can stand apart) aren't generated, skip them
            if (!zox_getn(child)) {
                free(g.voxels);
                continue;
            }
            const char* name = zox_getn(child);
            int solid, pieces, biggest;
            zox_tst_grid_pieces(&g, &solid, &pieces, &biggest);
            zox_log("[tst] model [%s] [%i] voxels a side, [%i] solid in [%i] pieces (biggest [%i])", name, g.length, solid, pieces, biggest);
            zox_tst_check(solid > 0, "model [%s] is empty", name);
            zox_tst_check(pieces == 1, "model [%s] is in [%i] pieces, [%i] voxels apart from its biggest", name, pieces, solid - biggest);
            for (int m = 0; m < zox_tst_shapes_mirrored_count; m++) {
                if (zox_tst_shapes_named(name, zox_tst_shapes_mirrored[m])) {
                    const int differ = zox_tst_grid_asymmetry(&g);
                    zox_tst_check(differ == 0, "model [%s] isn't mirrored left to right, [%i] voxels differ", name, differ);
                }
            }
            if (zox_tst_shapes_named(name, "Head")) {
                // the neck: the bottom layer, centred and narrower than the widest layer
                int bottom = -1, widest = 0, widest_y = 0, top = -1;
                int3 middle = { 0, 0, 0 };
                for (int y = 0; y < g.length; y++) {
                    int lower, upper;
                    zox_tst_grid_layer(&g, y, &lower, &upper);
                    if (upper < 0) {
                        continue;
                    }
                    if (bottom < 0) {
                        bottom = y;
                        middle.x = lower + upper;  // doubled
                    }
                    top = y;
                    if (upper - lower + 1 > widest) {
                        widest = upper - lower + 1;
                        widest_y = y;
                    }
                }
                int neck_lower, neck_upper;
                zox_tst_grid_layer(&g, bottom, &neck_lower, &neck_upper);
                const int neck_width = neck_upper - neck_lower + 1;
                int head_lower, head_upper;
                zox_tst_grid_layer(&g, widest_y, &head_lower, &head_upper);
                const int head_middle = head_lower + head_upper;  // doubled
                const int height = top - bottom + 1;
                // a neck is a column: its bottom fifth all narrow (a round chin's bottom tip is narrow, then widens fast)
                int neck_widest = 0;
                for (int y = bottom; y < bottom + int_max(2, height / 5); y++) {
                    int lower, upper;
                    zox_tst_grid_layer(&g, y, &lower, &upper);
                    neck_widest = int_max(neck_widest, upper - lower + 1);
                }
                zox_log("[tst] head: neck [%i] wide (its column up to [%i]), head [%i] wide at [%i], [%i] high",
                    neck_width, neck_widest, widest, widest_y - bottom, height);
                zox_tst_check(neck_widest * 2 <= widest, "the head's bottom fifth is up to [%i] wide of its widest [%i]: no neck",
                    neck_widest, widest);
                zox_tst_check(abs(middle.x - head_middle) <= 1, "the neck isn't under the middle of the head");
                // eyes (voxel 2) showing on its face: touching air on the front (+z)
                int eyes_seen = 0;
                for (int x = 0; x < g.length; x++)
                for (int y = 0; y < g.length; y++)
                for (int z = 0; z < g.length; z++) {
                    eyes_seen += zox_tst_grid_get(&g, x, y, z) == 2 && !zox_tst_grid_get(&g, x, y, z + 1);
                }
                zox_log("[tst] head: [%i] eye voxels on its face", eyes_seen);
                zox_tst_check(eyes_seen >= 2, "the head's eyes don't show on its face ([%i] voxels)", eyes_seen);
                zox_tst_check(widest_y - bottom >= height / 3, "the head's widest is at [%i] of [%i] high, not above a neck", widest_y - bottom, height);
            }
            if (zox_tst_shapes_named(name, "Chest")) {
                // each layer's solid voxels, a pinch is a layer under 70% of the smaller of the biggest below and above
                int areas[256];
                for (int y = 0; y < g.length; y++) {
                    areas[y] = 0;
                    for (int x = 0; x < g.length; x++)
                    for (int z = 0; z < g.length; z++) {
                        areas[y] += zox_tst_grid_get(&g, x, y, z) != 0;
                    }
                }
                int worst = 100, worst_y = -1;
                for (int y = 1; y < g.length - 1; y++) {
                    if (!areas[y]) {
                        continue;
                    }
                    int below = 0, above = 0;
                    for (int b = 0; b < y; b++) below = int_max(below, areas[b]);
                    for (int a = y + 1; a < g.length; a++) above = int_max(above, areas[a]);
                    const int around = int_min(below, above);
                    if (around > 0) {
                        const int percent = (100 * areas[y]) / around;
                        if (percent < worst) {
                            worst = percent;
                            worst_y = y;
                        }
                    }
                }
                char profile[512] = "";
                for (int y = 0; y < g.length; y++) {
                    char add[16];
                    snprintf(add, sizeof(add), " %i", areas[y]);
                    strcat(profile, add);
                }
                zox_log("[tst] chest layers (bottom up):%s", profile);
                zox_log("[tst] chest: narrowest layer [%i]%% of the layers around it, at [%i]", worst, worst_y);
                zox_tst_check(worst >= 70, "the chest pinches to [%i]%% at layer [%i]: its parts don't join", worst, worst_y);
            }
            if (zox_tst_shapes_named(name, "cokie")) {
                int chips[4] = { 0, 0, 0, 0 };
                for (int i = 0; i < g.length * g.length * g.length; i++) {
                    if (g.voxels[i] < 4) {
                        chips[g.voxels[i]]++;
                    }
                }
                zox_tst_check(chips[2] > 0 && chips[3] > 0, "the cookie has [%i] and [%i] chips of its two chocolates", chips[2], chips[3]);
                // crumbs: dough voxels outside the cookie's flat body (its widest and deepest layer's extent)
                int crumbs = 0;
                for (int x = 0; x < g.length; x++)
                for (int y = 0; y < g.length; y++)
                for (int z = 0; z < g.length; z++) {
                    // a crumb sits on the surface: dough touching air on 5 sides
                    if (zox_tst_grid_get(&g, x, y, z) != 1) {
                        continue;
                    }
                    int air = 0;
                    air += !zox_tst_grid_get(&g, x + 1, y, z);
                    air += !zox_tst_grid_get(&g, x - 1, y, z);
                    air += !zox_tst_grid_get(&g, x, y + 1, z);
                    air += !zox_tst_grid_get(&g, x, y - 1, z);
                    air += !zox_tst_grid_get(&g, x, y, z + 1);
                    air += !zox_tst_grid_get(&g, x, y, z - 1);
                    crumbs += air == 5;
                }
                zox_log("[tst] cookie: chips [%i] [%i], crumbs [%i]", chips[2], chips[3], crumbs);
                // the chips stand out from the dough: the hue turned round, or far brighter or darker
                const ColorRGBs* colors = zox_get(vox, ColorRGBs);
                zox_tst_check(colors->length >= 3, "the cookie has [%i] colours, not its dough and two chips", colors->length);
                if (colors->length >= 3) {
                    const float3 dough = color_rgb_to_hsv(colors->value[0]);
                    const float3 chip = color_rgb_to_hsv(colors->value[1]);
                    float hue_apart = fabsf(dough.x - chip.x);
                    hue_apart = hue_apart > 180 ? 360 - hue_apart : hue_apart;
                    const float value_apart = fabsf(dough.z - chip.z);
                    zox_log("[tst] cookie: dough hsv [%.0f %.0f %.0f] chips [%.0f %.0f %.0f], hues [%.0f] apart",
                        dough.x, dough.y, dough.z, chip.x, chip.y, chip.z, hue_apart);
                    zox_tst_check(hue_apart >= 120 || value_apart >= 30,
                        "the cookie's chips don't stand out: hues [%.0f] apart, values [%.0f]", hue_apart, value_apart);
                }
                zox_tst_check(crumbs > 0, "the cookie has no crumbs on it");
            }
            free(g.voxels);
            checked++;
        }
    }
    zox_tst_check(checked >= zox_tst_shapes_needed_count, "only [%i] models checked", checked);
}
