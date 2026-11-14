// the cubey gear's blueprints (doc/body_equips.md): boxes a margin bigger than the part they cover, trim painted on
//  - sizes are in body voxels (the chest's depth): gear is made at the chest's depth so its voxels line up

// gear blueprints are in body voxels at block_depth 4 (a body voxel a blueprint one), a finer Block Res doubles the body's
//  - body offsets (slot PartOffsets, made from block_depth) are converted to blueprint ones and back with these
static inline int gear_body_shift() {
    return (int) block_depth + 1 - (int) nodegraph_max_depth;
}

static inline int body_to_gear_blueprint(int body_voxels) {
    const int shift = gear_body_shift();
    return shift >= 0 ? body_voxels >> shift : body_voxels << -shift;
}

static inline int gear_blueprint_to_body(int blueprint_voxels) {
    const int shift = gear_body_shift();
    return shift >= 0 ? blueprint_voxels << shift : blueprint_voxels >> -shift;
}

// links a blueprint's nodes after its cloth colour, then its end node
static inline entity link_blueprint_nodes(ecs* world, color cloth_color, entity* nodes, byte count) {
    nodes[count++] = spawn_node_model_end(world);
    entity e = spawn_node_model_colors(world, cloth_color, 1);
    entity previous = e;
    for (byte i = 0; i < count; i++) {
        new_link_single_node(world, previous, nodes[i]);
        previous = nodes[i];
    }
    return e;
}

static inline entity spawn_blueprint_box(ecs* world, byte3 corner, byte3 size, byte voxel, byte depth) {
    const entity e = spawn_node_model_at(world, prefab_node_model, zox_model_node_box, corner, size, voxel);
    zox_setv(e, NodeDepth, depth);
    return e;
}

// bevels a box's 4 edges along an axis (0 x, 1 y, 2 z): a stepped cut bevel voxels deep, a cube with soft corners
static inline byte spawn_blueprint_bevel(ecs* world, entity* nodes, byte count, byte3 corner, byte3 size, byte axis, byte bevel, byte depth) {
    const byte a = axis == 0 ? 1 : 0, b = axis == 2 ? 1 : 2;   // the two axes across the edge
    const byte* sizes = (const byte*) &size;
    for (byte i = 0; i < 4; i++) {
        // a step a voxel across: bevel 2 cuts 2 voxels in from each face at the corner, 1 beside it
        for (byte step = 0; step < bevel; step++) {
            byte3 cut_corner = corner;
            byte3 cut_size = byte3_single(1);
            byte* c = (byte*) &cut_corner;
            byte* n = (byte*) &cut_size;
            n[a] = bevel - step;
            c[a] += (i & 1) ? sizes[a] - n[a] : 0;
            c[b] += (i & 2) ? sizes[b] - 1 - step : step;
            n[axis] = sizes[axis];
            nodes[count++] = spawn_blueprint_box(world, cut_corner, cut_size, 0, depth);
        }
    }
    return count;
}

// paint is a corner and a size (inclusive), over voxels already there
static inline entity spawn_blueprint_paint(ecs* world, byte3 corner, byte3 size) {
    return spawn_node_model_at(world, prefab_node_model, zox_model_node_paint, corner, size, 2);
}

// a shirt, worn over a chest, cubey but shaped like it: a box round each of the chest's blobs (get_chest_blobs), so it
//  steps out from the belly to the ribs to the shoulders, edges bevelled, and a sleeve over each shoulder
//  - chest_nsize: the chest's blueprint size, its blobs are worked out from it like the chest's own
//  - sleeves (reach 0 for none): a cap over the shoulder out to its tip, reach past the chest, and a tube hanging from
//    it round the top of the upper arm, its cuff facing down (the arms hang, a sleeve straight out reads as a t-pose)
//  - shoulder_size and shoulder_y: the shoulders' bounds and bottom (in the chest), arm_x and arm_width: the upper
//    arm's from the shoulder's tip, drop: how far the tube hangs below the shoulder
//  - trim: a hem, a collar round the neck, cuffs, buttons down its front (+z)
//  - its voxels start at 0: a body centres a part by its size, and copies its voxels from 0 (build_body_parts)
entity spawn_blueprint_shirt(
    ecs* world,
    byte3 chest_nsize,
    byte3 shoulder_size,
    byte shoulder_y,
    byte depth,
    color cloth_color,
    color trim_color,
    byte margin,
    byte reach,
    byte arm_x,
    byte arm_width,
    byte drop)
{
    chest_blob blobs[chest_blobs_count];
    get_chest_blobs(chest_nsize, blobs);
    byte3 corners[chest_blobs_count], sizes[chest_blobs_count];
    byte3 extent = byte3_zero;      // the chest's voxels' bounds, the shirt's (less its sleeves)
    for (byte i = 0; i < chest_blobs_count; i++) {
        chest_blob_box(blobs[i], &corners[i], &sizes[i]);
        // the margin round each, the shirt's own corner at 0 (offset by the sleeves on x)
        corners[i] = (byte3) { corners[i].x + reach, corners[i].y, corners[i].z + margin };
        sizes[i] = (byte3) { sizes[i].x + 2 * margin, sizes[i].y, sizes[i].z + 2 * margin };
        corners[i].x -= margin;
        corners[i].z -= margin;
        extent.y = corners[i].y + sizes[i].y > extent.y ? corners[i].y + sizes[i].y : extent.y;
        extent.z = corners[i].z + sizes[i].z > extent.z ? corners[i].z + sizes[i].z : extent.z;
    }
    extent.x = sizes[2].x;          // the yoke is the widest
    const byte width = extent.x + 2 * reach;
    // a cap runs in from the shoulder's tip to the ribs' box, no shoulder shows between them
    const byte3 cap_size = { corners[1].x + 1, shoulder_size.y + 2 * margin, shoulder_size.z + 2 * margin };
    const byte cap_y = shoulder_y > margin ? shoulder_y - margin : 0;
    const byte cap_z = (extent.z - cap_size.z) / 2;
    const byte tube_width = arm_width + 2 * margin;
    const byte tube_x = arm_x > margin ? arm_x - margin : 0;
    const byte tube_y = cap_y > drop ? cap_y - drop : 0;
    const byte3 tube_size = { tube_width, cap_y - tube_y, cap_size.z };
    const byte neck = sizes[2].x / 3;
    const byte3 neck_corner = { width / 2 - neck / 2, extent.y - 2, extent.z / 2 - neck / 2 };
    entity nodes[48];
    byte count = 0;
    // cloth: the belly, ribs and yoke boxes, sleeves, the neck cut (voxel 0), then bevels
    for (byte i = 0; i < chest_blobs_count; i++) {
        nodes[count++] = spawn_blueprint_box(world, corners[i], sizes[i], 1, depth);
    }
    const byte3 caps[] = { { 0, cap_y, cap_z }, { width - cap_size.x, cap_y, cap_z } };
    const byte3 tubes[] = { { tube_x, tube_y, cap_z }, { width - tube_x - tube_width, tube_y, cap_z } };
    for (byte i = 0; i < 2 && reach; i++) {
        nodes[count++] = spawn_blueprint_box(world, caps[i], cap_size, 1, depth);
        nodes[count++] = spawn_blueprint_box(world, tubes[i], tube_size, 1, depth);
    }
    // the belly's and ribs' upright edges (the ribs' in steps), the yoke's and caps' long ones, the tubes' upright ones
    count = spawn_blueprint_bevel(world, nodes, count, corners[0], sizes[0], 1, 1, depth);
    count = spawn_blueprint_bevel(world, nodes, count, corners[1], sizes[1], 1, 2, depth);
    if (!reach) {
        count = spawn_blueprint_bevel(world, nodes, count, corners[2], sizes[2], 0, 1, depth);
    }
    for (byte i = 0; i < 2 && reach; i++) {
        // only out past the chest, else it would cut into the ribs
        count = spawn_blueprint_bevel(world, nodes, count, (byte3) { i ? width - reach : 0, cap_y, cap_z },
            (byte3) { reach, cap_size.y, cap_size.z }, 0, 1, depth);
        count = spawn_blueprint_bevel(world, nodes, count, tubes[i], tube_size, 1, 1, depth);
    }
    nodes[count++] = spawn_blueprint_box(world, neck_corner, (byte3) { neck, 2, neck }, 0, depth);
    // trim: the hem, the collar, the cuffs round the tubes' bottoms, buttons
    nodes[count++] = spawn_node_model_colors(world, trim_color, 1);
    nodes[count++] = spawn_blueprint_paint(world, corners[0], (byte3) { sizes[0].x - 1, 0, sizes[0].z - 1 });
    nodes[count++] = spawn_blueprint_paint(world, (byte3) { neck_corner.x - 1, extent.y - 1, neck_corner.z - 1 },
        (byte3) { neck + 1, 0, neck + 1 });
    for (byte i = 0; i < 2 && reach; i++) {
        nodes[count++] = spawn_blueprint_paint(world, tubes[i], (byte3) { tube_size.x - 1, 0, tube_size.z - 1 });
    }
    nodes[count++] = spawn_blueprint_paint(world, (byte3) { width / 2, 2, corners[1].z + sizes[1].z - 1 },
        (byte3) { 0, extent.y - 6, 0 });
    return link_blueprint_nodes(world, cloth_color, nodes, count);
}

// shorts, worn over the hips:
//  - a waist box, margin wider than the hips each side, as tall as them
//  - a leg under it over each thigh, leg voxels long, a margin around the thigh
//  - trim: a waistband, hems round the legs' bottoms, a drawstring in front (+z)
//  - thigh_x: each thigh's corner from the hips' (bottom anchor, its PartOffset), right then left
entity spawn_blueprint_shorts(
    ecs* world,
    byte3 hips_size,
    byte3 thigh_size,
    byte2 thigh_x,
    byte depth,
    color cloth_color,
    color trim_color,
    byte margin,
    byte leg)
{
    const byte3 waist = { hips_size.x + 2 * margin, hips_size.y, hips_size.z + 2 * margin };
    const byte3 leg_size = { thigh_size.x + 2 * margin, leg, thigh_size.z + 2 * margin };
    const byte leg_z = (waist.z - leg_size.z) / 2;
    // a leg's corner is its thigh's, less the margin, plus the waist's margin
    const byte legs_x[] = { thigh_x.x, thigh_x.y };
    entity nodes[32];
    byte count = 0;
    nodes[count++] = spawn_blueprint_box(world, (byte3) { 0, leg, 0 }, waist, 1, depth);
    // rounded: the waist's upright edges (hips curving round), each leg a round tube
    count = spawn_blueprint_bevel(world, nodes, count, (byte3) { 0, leg, 0 }, waist, 1, 2, depth);
    for (byte i = 0; i < 2; i++) {
        nodes[count++] = spawn_blueprint_box(world, (byte3) { legs_x[i], 0, leg_z }, leg_size, 1, depth);
        count = spawn_blueprint_bevel(world, nodes, count, (byte3) { legs_x[i], 0, leg_z }, leg_size, 1, 1, depth);
    }
    nodes[count++] = spawn_node_model_colors(world, trim_color, 1);
    nodes[count++] = spawn_blueprint_paint(world, (byte3) { 0, leg + waist.y - 1, 0 }, (byte3) { waist.x - 1, 0, waist.z - 1 });
    for (byte i = 0; i < 2; i++) {
        nodes[count++] = spawn_blueprint_paint(world, (byte3) { legs_x[i], 0, leg_z }, (byte3) { leg_size.x - 1, 0, leg_size.z - 1 });
    }
    nodes[count++] = spawn_blueprint_paint(world, (byte3) { waist.x / 2 - 1, leg + waist.y - 3, waist.z - 1 }, (byte3) { 2, 1, 0 });
    return link_blueprint_nodes(world, cloth_color, nodes, count);
}

// a shoe, worn over a foot (one a foot), shaped like one: low at the toe, up round the ankle at the back
//  - a margin wider and longer than the foot, its heel height tall, its toe a voxel lower (the foot under it)
//  - rounded: its sides' upright edges, its toe's top 2 deep (a round toe), its heel's back edges, the step between
//  - trim: a sole, a collar round the heel's top, laces down the toe's top (+z its front)
entity spawn_blueprint_shoe(
    ecs* world,
    byte3 foot_size,
    byte depth,
    color cloth_color,
    color trim_color,
    byte margin,
    byte height)
{
    const byte3 size = { foot_size.x + margin, height, foot_size.z + margin };
    const byte3 toe = { size.x, height > 2 ? height - 1 : height, size.z };
    const byte3 heel = { size.x, height, size.z / 2 };
    entity nodes[40];
    byte count = 0;
    nodes[count++] = spawn_blueprint_box(world, byte3_zero, toe, 1, depth);
    count = spawn_blueprint_bevel(world, nodes, count, byte3_zero, toe, 0, toe.y > 2 ? 2 : 1, depth);
    nodes[count++] = spawn_blueprint_box(world, byte3_zero, heel, 1, depth);
    count = spawn_blueprint_bevel(world, nodes, count, byte3_zero, heel, 0, 1, depth);
    // a flat sole under it, the toe and heel bevels cut its bottom edges
    nodes[count++] = spawn_blueprint_box(world, byte3_zero, (byte3) { size.x, 1, size.z }, 1, depth);
    count = spawn_blueprint_bevel(world, nodes, count, byte3_zero, size, 1, 1, depth);
    nodes[count++] = spawn_node_model_colors(world, trim_color, 1);
    nodes[count++] = spawn_blueprint_paint(world, byte3_zero, (byte3) { size.x - 1, 0, size.z - 1 });
    nodes[count++] = spawn_blueprint_paint(world, (byte3) { 0, height - 1, 0 }, (byte3) { size.x - 1, 0, heel.z - 1 });
    nodes[count++] = spawn_blueprint_paint(world, (byte3) { size.x / 2 - 1, toe.y - 1, heel.z }, (byte3) { 1, 0, (size.z - heel.z) / 2 });
    return link_blueprint_nodes(world, cloth_color, nodes, count);
}
