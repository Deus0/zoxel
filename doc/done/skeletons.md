# Skeletons: Bone Weights From an Octree

How a skeleton character's mesh vertices get their bones now, why that's fragile, and a plan to store each voxel's bone
in an octree written when the parts merge. Written 2026-11-14 from reading the code, done 2026-11-15 (below).

Code: [src/parts/sys/combine.c](../../src/parts/sys/combine.c) (`body_combine_system`, `build_body_parts`),
[src/chunks/chunks3/sys/combine.c](../../src/chunks/chunks3/sys/combine.c) (`combine_vox_system`),
[src/parts/sys/bones.c](../../src/parts/sys/bones.c) (`character_bone_spawn_system`, `spawn_part_bones`),
[src/bones/sys/generate.c](../../src/bones/sys/generate.c) (`generate_weights`),
[src/bones/sys/skeleton_meshes.c](../../src/bones/sys/skeleton_meshes.c),
[src/chunks/chunks3/sys/vox_lods.c](../../src/chunks/chunks3/sys/vox_lods.c) (`build_vox_mesh_at_depth`),
[src/chunks/chunks3/sys/build_colored.c](../../src/chunks/chunks3/sys/build_colored.c) (the face builders),
[res/shaders/bone.vert](../../res/shaders/bone.vert), tests `chest_weights`, `npc_skeletons`, `skeleton_render`.
Also [character.md](../tech/characters.md), [body_equips.md](../tech/body_equips.md).

## Done (2026-11-15, commit ee11b43a3; clothes and double faces 2026-11-18, below)

- Analysis first (chest_weights, a humanoid, its active mesh): every one of 5382 solid voxels got a part, and the box
  weights gave 1153 of 18268 vertices another bone than their voxel's, at every joint: chest for hips 149, chest for a
  shoulder 146, hips for the thighs 214, head for the chest 86, a shoulder for its arm 78.
- `BoneNode` (chunks3/com/bones.c) is written by `combine_vox_system`: a voxel's CombineList entry + 1; clothes
  (`CombineOver`) keep the part under them, past the body the nearest part within 3 voxels (else their own entry).
  Parts sets `PartIndex` on each slot as it's placed; `spawn_part_bones` maps it to the slot's bone in `CombineBones`
  (clothes: their parent's bone).
- Weights (`generate_weights_parts`, bones/sys/generate.c): no change to the mesh builder. A face's voxel is found
  half a voxel behind its middle (the solid side); each vertex's corner has 8 voxels, a joined bone (parent or child of
  the face's) among them blends in by its share: `BoneIndexes` is byte4 { bone, other, weight, 0 }, `bone.vert` mixes
  the two bone transforms. A plain joint plane meets at 50/50 from both sides, so the skin stays closed; an arm resting
  on the chest isn't joined to it and doesn't blend. 977 vertices blend on that humanoid.
- Closed at every corner (2026-11-15, the user saw holes at the shoulders animating): a vertex takes the two bones
  most of its corner's 8 voxels are, of those joined to its face's bone directly or through each other (chest,
  shoulder, bicep), the lower bone first when equal, so every face at a corner gets the same two and weight. A face
  merged across voxels can reach a corner none of its bone's voxels touch: it takes that corner's own bones (it gave
  it the face's bone: 7 cracked corners a humanoid, both shoulders). A coarser lod's voxel takes the bone most of
  its children are (`bone_parts_most`; an open node's own value is stale). `tst chest_weights` counts corners
  whose faces move them differently: 0.
- Cost: an octree walk a lookup made it 31-36ms a body; the parts are flattened to a grid at the mesh's depth once
  (`bone_parts_fill`), 6.4ms at worst (the box weights were 12ms), 0.15ms a frame on average.
- Box weights (`bone_vertex_score`) are gone. `BoneSize` stays for the head camera and the debug boxes.
- Holes as parts move apart (2026-11-16, tst `body_poses`, `bash bsh/poses.sh`): the body is one vox, so the faces
  where two parts only touch (an arm against the chest, the shirt's hem on the thighs) were culled; moved apart, the
  skin was open there (an arm raised opened the chest's side). Skeleton meshes now keep a face between voxels whose
  bones aren't the same, parent or child (`vox_mesh_parts`, chunks3/sys/build_colored.c, both the grid and the octree
  mesher: Block Res 5 is depth 8, past the grid's 7), and those parts don't shade each other's ao. A face's bone takes
  its normal from its triangle's indicies (`bone_parts_face`): opposite faces list their corners in the same order,
  so those new faces took the other part's bone. A joint's blend weight is the lower bone's share either way round
  (rounding each order apart left hairline cracks bent 90 degrees). The test checks every edge of the rest mesh is
  closed per joined piece (242 open before), no joint corner parts in 12 poses, and saves the poses' screenshots.
- Clothes' parts and no double faces (2026-11-18, the user's play test: skinning very deformed). The painted view
  (tst `body_weights`, `bash bsh/poses.sh --weights`, the Paint Weights setting: bone.vert colours each vertex by its
  bones) showed sleeves split diagonally between shoulder and bicep, a thigh's part up the shorts into the shirt: a
  clothes voxel past the body took the first part found within 3 voxels, in loop order, and one resolved counted for
  the next, so parts crawled along the clothes. Now `combine_parts_resolve` floods out from the body a layer at a
  time (a voxel takes the most of its 6 neighbours' parts from the layers before), so each clothes voxel takes the
  part it's nearest to through the clothes. With the parts clean, the touching-parts faces (`vox_mesh_parts`) went:
  parts not joined (`CombineParents`, an entry's parent entry, set by parts) never touch, a voxel's gap left between
  them (a body voxel isn't placed there, a clothes voxel is taken out), so every culled face is between joined parts.
  body_poses: 0 open edges at Block Res 4 and 5 (242 with the old resolve, no double faces). The combine works on a
  flat grid of parts the body's size (octree lookups made it 26ms, now 12ms at Block Res 5, it was 11ms).
- Later: the blend is a voxel wide (the corner); wider (sample further from the corner) if joints bend sharply;
  cloth simulation for capes and skirts (they follow the bone under them for now, the user is fine with that).

## How it works now

1. **Combine** (`body_combine_system`, parts): walks the body's slots from the chest (`build_body_parts`), places each
   part by its anchor (top, right, over...) and writes its `PartPosition` / `PartSize` on the slot. Clothes (anchor
   over) go last among siblings so they draw over what they cover. The result is three parallel lists on the body's
   vox: `CombineList` (each part's model vox), `CombinePositions`, `CombineDepths`. Then `BodyDirty` = bones.
2. **Voxels** (`combine_vox_system`, chunks3): clears the vox's `VoxelNode`, and for each list entry copies every voxel
   of that part's model into it at its position (colours merged into one palette). **This is the one place that knows
   which part every voxel came from**, and it throws that away.
3. **Bones** (`character_bone_spawn_system`, parts): deletes the old bones, walks the slots again (`spawn_part_bones`,
   its own order) and spawns a bone per slot holding a part, except clothes. A bone's `BonePosition` is its pivot, the
   joint with its parent's part (`part_bone_pivot`: between the two parts' facing sides, on the axis they're furthest
   apart on, the middle of their overlap on the others; the chest, the root, its centre), `BoneCentre` the slot's
   centre, its `BoneSize` the part model's half size plus half a voxel. At their centres the limbs turned about their
   middles: a bent elbow pulled the forearm a gap off the upper arm, the skin between in strips (tst punch_views). The `BoneLinks` index is the bone's index in
   the shader. Then `SkeletonDirty`.
4. **Meshes** (`skeleton_mesh_build_system`): builds a mesh per render depth from the vox (`build_vox_mesh_at_depth`:
   4 vertices a face, no sharing), then `BuildMeshWeights`.
5. **Weights** (`bone_index_generate_system2` → `generate_weights`): for every vertex, every bone gets a score from its
   box (`bone_vertex_score`: inside, how deep, under 0; outside, how far). The lowest wins. One bone a vertex
   (`BoneIndexes`, a byte), rigid: `bone.vert` takes `bone_matrix[bone_index]`. `SkeletonDirty` reweights every mesh
   (`bone_index_generate_system`).

## Problems with the boxes

- **Boxes overlap, and the shapes aren't boxes.** A box is sized from the part's model size, padded, so neighbouring
  boxes overlap at every joint, and a rounded chest or tapered limb doesn't fill its box. Each fix so far has been a
  rule on top:
  - "the last box that holds it" put 778 rib vertices on the shoulders, and the chest came apart when the arms swung
    (fc4e0cf20: deepest box wins)
  - "outside every box, the chest" left an arm's front strip still while its sleeve swung (33a5b7412: nearest box)
  - clothes can't have a bone: a shirt's box, wider than the chest, took the shoulders' vertices (`spawn_part_bones`
    returns 0 for anchor over), so clothes rely on lying inside the right boxes
- **A vertex isn't a voxel.** A face's corners sit on its voxel's edges; on a part boundary a corner can score to the
  next part's box, so one face can stretch across two bones. Which bone a face belongs to is known exactly (its voxel)
  but recomputed from geometry.
- **It ignores what the merge knew.** Overlaps, padding and "nearest" all try to recover which part a voxel came
  from, which `combine_vox_system` had in hand.
- **Cost:** vertices × bones per mesh per depth (a few thousand vertices × ~15 bones), again on every `SkeletonDirty`,
  plus a pipeline stage (`BuildMeshWeights`) between building and uploading.
- `BoneSize` is also used by `head_camera` (eye height) and `bone_render` (the debug boxes): those stay.

## The plan (as proposed, done above with small changes)

### Data

- A `BoneNode` octree on the combined vox: `zoxc_octree_basic(BoneNode, byte, 0)`, like `LightOctree` and `SidesOctree`
  sit beside `VoxelNode`. A voxel's value is its bone's index + 1 (0 = none / air). Same depth as `VoxelNode`.
- Locked with the vox's octree lock (`zox_octree_lock`): written and read together with `VoxelNode` (thread_safety.md).
- Lods: a parent's value is the most common of its children, air skipped (`octree_children_mode`, what
  `optimize_reduce_VoxelNode` does for voxels), so a mesh at a lower depth reads its voxel's bone the same way it
  reads its colour. Reduce collapses whole regions of one bone (a forearm is one leaf), so it stays small.

### Writing it (the merge)

- `build_body_parts` (parts) already walks slots in an order with parents before children. Spawn the bones in that
  same order (or record, per combine entry, the bone it will get), so each `CombineList` entry has a bone index.
  Add `CombineBones` (a byte array beside `CombinePositions`, chunks3 so `combine_vox_system` can read it, filled by
  parts: module order chunks3 < bones < parts holds):
  - a part with a bone: its bone index + 1
  - clothes (anchor over): 0, "the bone of what it covers"
- `combine_vox_system`: when it places a voxel, also `set_BoneNode` at the same position:
  - a part's voxel: its entry's bone
  - a clothes voxel over a body voxel: keep the bone already there (the body was placed first)
  - a clothes voxel past the body (a sleeve's outer layer, a skirt): the bone of the nearest body voxel, searched
    outwards a voxel at a time up to a small radius (the clothes' thickness, 2-3); none found, its parent slot's bone
  - after the loop: optimize + reduce the `BoneNode`, as voxels are
- Bones then don't need boxes for weights at all. `BonePosition` stays (the pivot), `BoneSize` stays for the camera and
  debug.

### Reading it (the mesh build)

- The face builders (`build_voxel_faces_colored` via `build_voxel_mesh_c` and the pyramid path
  `build_voxel_mesh_grid`) know each face's voxel position at the target depth. With an optional `BoneNode` and a
  bones array in `mesh_colored_build_data`, each face pushes its voxel's bone 4 times (`getv_BoneNode` at that
  depth, or walked alongside the voxel node).
- `skeleton_mesh_build_system` fills `BoneIndexes` while building, sets `SkeletonMeshDirty` itself, and
  `BuildMeshWeights`, `bone_index_generate_system2` and `generate_weights` go. A `SkeletonDirty` with the same
  voxels no longer reweights anything; a new body recombines, which rebuilds the meshes anyway.
- Terrain and plain vox meshes pass no `BoneNode`, unchanged.

## Analysis first (before changing weights)

The ground truth exists at the merge, so measure the current weights against it before replacing them:

1. Write `BoneNode` in `combine_vox_system` (and `CombineBones`) without using it yet.
2. A test (`bone_octree`, or in `chest_weights`): spawn a humanoid in its usual clothes; for each skeleton mesh face,
   compare `generate_weights`' bone with `BoneNode` at the face's voxel. Log how many faces differ, by bone pair
   (chest/shoulder, shoulder/arm, thigh/hips, the shirt's sleeves) and by depth. Also check the octree itself: every
   body voxel has a bone, each part's voxels have its bone, clothes voxels the bone under them.
3. Then switch the mesh build to `BoneNode`, keep the test as the guard (every face's bone = its voxel's), and keep
   `chest_weights` (arms and ribs move with the right bones).

## Steps

1. `BoneNode` component and `CombineBones`; bones spawned in combine order (or the entry → bone map). No behaviour
   change. The analysis test above. One commit.
2. Mesh build reads it; remove `generate_weights` and `BuildMeshWeights`. `chest_weights`, `npc_skeletons`,
   `skeleton_render`, `body_views`, `head_camera`, `held_item` pass. One commit.
3. Update body_equips.md (its "last bone whose box (1.2 times its size)" line is already out of date) and
   character.md's pipeline line.

## Open questions

- **Joints:** a face is now wholly one bone (rigid). Between a chest voxel and a shoulder voxel the faces stay
  together while still, but a swing can open a crack at the joint where today a stretched face hid it. If it shows:
  give the vertices on a part boundary both bones (two indices and a weight, a shader change), or round joints in
  the models. Worth checking in the analysis (count faces whose corners touch another bone's voxels).
- **Lods:** the most common bone of 8 children can put a small part's tip (a finger, a hat brim) on its neighbour at
  low depths. Far away, likely unseen.
- **Clothes past the body:** is nearest the right rule for a skirt or a cape (they'd follow the thighs or chest)? A
  slot could name its own bone later (capes, tails, hair).
- **Held items** stay separate entities on the hand bone (`HandBoneLink`), unaffected.
- **Animations exported to Blender** (todo.md) would want real per voxel bones: this gives them.
