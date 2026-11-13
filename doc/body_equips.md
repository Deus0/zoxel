# Body and Equipment

How a character's body is built from parts, and how gear (hats, shirts, shorts, shoes) is worn on it.

- Parts: [src/parts](../src/parts)
- Equipment: [src/equipment](../src/equipment)
- Slots: [src/slots](../src/slots)
- The realm's parts and gear: [gam/zoxel/sys/body_parts.c](../gam/zoxel/sys/body_parts.c) and
  [gam/zoxel/sys/equipment.c](../gam/zoxel/sys/equipment.c)

## The idea

A body is a tree of **slots**. Each slot holds an **item** (a part or gear) through `DataLink`.

Every item has a vox model. The body merges all of their voxels into one vox, then gives that vox a bone per part.
To change how a character looks, change what is in its slots and set `BodyDirty`.

```text
character
└─ chest slot (Body, anchor core) ── DataLink → chest item
   ├─ shirt slot (EquipSlot, ShirtSlot, anchor over) → shirt item
   ├─ head slot (anchor top) → head
   │  └─ hat slot (EquipSlot, HatSlot, anchor top) → hat item
   ├─ hips slot (anchor bottom) → hips
   │  ├─ shorts slot (EquipSlot, ShortsSlot, anchor over, PartOffset down zox_shorts_leg) → shorts item
   │  └─ thighs → calves → feet
   │                       └─ shoe slot (EquipSlot, ShoeSlot, anchor over), one a foot → shoes item
   └─ shoulder slots (anchor left / right) → shoulders
      └─ biceps → forearms → hands
```

## Entities

| Entity | What it is | Made in |
|---|---|---|
| Realm part item | A body part kind (Chest, Head...), `BodyItem` `BodyPart`, `SlotType`, a model and icon | `bodys_realm_spawn_system`, from blueprints |
| Realm gear item | A gear kind, `EquipItem`, a `Hat`, `Shirt`, `Shorts` or `Shoes` tag | `equips_realm_spawn_system` (hats from vox files, clothes from blueprints) |
| User item | One character's copy of a realm item (an instance of it) | `spawn_user_item` |
| Slot | Where an item attaches: `SlotAnchor`, `PartOffset`, then `PartPosition` and `PartSize` once built | `spawn_body_slot`, `spawn_equip_slot` (both `spawn_attach_slot`) |
| Body vox | The character's merged voxels (`VoxLink`), `CombineList` of the parts' voxes | the character |
| Bones | One a part slot, sized and placed from the slot | `character_bone_spawn_system` |

## Making the realm's parts

- The **chest** (`spawn_blueprint_chest`) is three ellipsoid blobs, a v like a person's: a narrow belly, wider ribs,
  and a full width yoke where the shoulders join (`get_chest_blobs`, shared with the shirt). It's flatter front to back
  than across (`player_chest_scale`). The shoulders sit level with its top (`body_shoulder_offset_y`) and dig into its
  sides (`body_shoulder_dig`, scaled with Block Res so a finer body keeps its shape).
- **Blueprints** are node graphs: a colours node, fill nodes (ellipsoids around a centre), box nodes (a box from a
  corner, voxel 0 cuts air), paint nodes
  (a corner box, they recolour voxels already there), and an end node.
  - They are made at the fixed nodegraph depth (`nodegraph_max_depth`, 32 voxels), and shifted to the model's depth.
  - See `src/parts/ins/chest.c` and `head.c`.
- `spawn_realm_blueprint_item`: a model group, its generated model, an icon texture and the realm item.
  - Body parts tag it (`spawn_realm_body_part`).
  - Gear made from a blueprint tags it `EquipItem` (`spawn_realm_item_equip_blueprint`).
- A model's size is the bounds of its voxels (`ChunkSize`, set when its node graph ends).

## Building a character's body

Each step sets the next by the character's `BodyDirty` and `GenerateCharacter`.

1. `character_body_spawn_system` (`src/parts/sys/character.c`), when the character generates:
   - It finds the realm's part items by `SlotType` and links the chest to the chest slot.
   - It spawns the other slots with their anchors and offsets, and links a user item in each.
   - It also makes the empty gear slots: the shirt over the chest, shorts over the hips, a shoe over each foot, the hat
     on the head.
2. `character_player_equips_system` (`src/equipment/sys/character.c`), players only, when generation ends:
   - `equip_realm_item` puts the realm's first `Hat`, `Shirt`, `Shorts` and `Shoes` in every slot with the matching tag
     (a shoe in each `ShoeSlot`).
   - Then it sets `BodyDirty` again.
3. `body_combine_system` (`src/parts/sys/combine.c`): `build_body_parts` walks the slots from the chest, depth first.
   - It places each part from its parent slot by its anchor.
   - A part below 0 shifts everything placed before it, and the body's size grows to fit.
   - It fills the body vox's `CombineList`, then `combine_vox_system` merges them.
   - **Later parts overwrite earlier ones**, so a slot's children are drawn over it, and clothes (anchor over) last.
4. `character_bone_spawn_system` (`src/parts/sys/bones.c`): a bone a part slot.
   - Each bone gets the slot's position and size, and is tagged by the part's tags (`Head`, `Shoulder`, `Hand`...).
   - Every mesh vertex takes the **last** bone whose box (1.2 times its size) holds it (`generate_weights`).
   - Gear gets a bone too, as any slot holding an item does: a hat's bone is a child of the head's, so it moves
     with it. Clothes (anchor over) get none, or a shirt's bone, wider than the chest, would take the shoulders' voxels.

## Anchors

`SlotAnchor`, in `src/parts/_.c`. Every anchor adds `PartOffset` to where it would be.

| Anchor | Where a part goes |
|---|---|
| core | The body's root (the chest) |
| top | On top of the parent, centred |
| bottom | Under the parent, centred |
| left, right | Beside the parent, centred |
| forward, back | In front of or behind the parent, centred |
| over | **Covers the parent** (clothes): centred on x and z, from its bottom up. Bigger than its parent, it sticks out. No bone of its own: it moves with its parent. |

## Gear

- **Gear slots** are attach slots tagged `EquipSlot`, plus what they hold (`HatSlot`, `ShirtSlot`, `ShortsSlot`,
  `ShoeSlot`).
- The equipment menu (`spawn_player_menu_equipment`) lists every `EquipSlot` on the body.
  - Dropping an item there only checks it is an `EquipItem`.
  - A change sets `BodyDirty`, and the body rebuilds.
- **Hats** are vox files (`hatty`, `cavalier_hat`, `top_hat`), cloned into the realm (`spawn_realm_model_item_filename`).
- **Clothes** are the cubey gear, made from blueprints of box nodes (`src/parts/ins/clothes.c`), realm items in
  `gam/zoxel/sys/equipment.c`:
  - Each box covers a part's bounds, `margin` more each side. For now one model fits every character (later, one a
    character from the blueprint, fitted to its chest). Body parts are ellipsoids, so a box their size already
    hides them: the shirt, shorts and shoes use margin 0 (1 in all for shoes, so they centre on the foot).
  - Sizes are in blueprint voxels, a body voxel each at Block Res 4 (a finer Block Res doubles the body, and the gear's
    model with it). Offsets from the body (made from `block_depth`) convert with `body_to_gear_blueprint` and
    `gear_blueprint_to_body`, or at Block Res 5 they come out twice too big (a negative byte froze loading, 2026-11-13).
  - Sizes are worked out from the parts' blueprint scales (`body_part_voxels`). The limbs' are half
    their blueprint, and the rounding can be a voxel off.
  - At the chest's depth, so the voxels line up. Their colours come from the realm's seed (a cloth and a trim colour).
  - **Shirt** (`spawn_blueprint_shirt`), cubey but shaped like the chest: a box round each of the chest's blobs
    (`get_chest_blobs`), so it steps out from the belly to the ribs to the shoulders, a neck hole. Each sleeve is a
    cap over the shoulder, from its tip in to the ribs, and a tube hanging from it round the top of the upper arm, its
    cuff facing down (a sleeve straight out read as a t-pose). Edges are bevelled (`spawn_blueprint_bevel`, voxel 0
    cuts). Trim: hem, collar, cuffs, buttons. The realm has a **Shirt** (worn) and a sleeveless **Vest** (inventory).
  - **Shorts** (`spawn_blueprint_shorts`): a waist box over the hips, a leg over each thigh, `zox_shorts_leg` long.
    Their slot's `PartOffset` drops them that far, so the waist lines up with the hips. Trim: waistband, hems,
    drawstring.
  - **Shoes** (`spawn_blueprint_shoe`): one shoe, worn in both feet's slots, up all but the foot's top voxel. Its
    upright, toe and heel edges are bevelled.
    Trim: sole, collar, laces.
  - They sit over their part (anchor over). Clothes merge after their sibling slots, so they cover them.
  - They move with the limbs they cover: a vertex in a bone's box takes that bone (sleeves the shoulders', shorts' legs
    the thighs', shoes the feet's).

## Adding gear

1. Make its model.
   - A blueprint like `spawn_blueprint_shirt`, then `spawn_realm_gear` (`spawn_realm_item_equip_blueprint`).
   - Or a vox file, then `spawn_realm_model_item_filename`.
2. Tag what it is (`Shirt`).
3. Add its slot where its parent slot is made (`character_body_spawn_system`), with an anchor and a tag (`ShirtSlot`).
   - Clothes use `body_anchor_over`.
4. To give it to players, call `equip_realm_item` with the slot and item tags.
5. Name its `SlotType` in `slot_names` (tooltips).
6. Test it in `tst clothes`, and look at the body it logs.

## Tests

- `parts`: every realm part item's model has voxels, and its icon has pixels.
- `clothes` (in game):
  - the player wears a shirt over the chest, shorts over the hips and a shoe on each foot, as wide as their parts or wider
  - their colours are in the body, and none have a bone of their own
  - logs every part's place and size, the body from the front and side, and a slice through each piece from above
    (S shirt, P shorts, B shoes, o skin)
- `npc_skeletons`: npc bodies and bones.

## Problems found

Logged in [refactors.md](refactors.md). Fixed: the second hat never reached the inventory (an inverted check),
`slot_names` was out of order, and calves had the thighs' slot types.

- Realm parts are found by `SlotType` ids (`find_slot_type`), against
  [Entities, Not Ids](coding_standards.md#entities-not-ids).
  - Gear is found by tag now (`Hat`, `Shirt`).
  - Parts could be too, or the realm could link each part kind.
- `character_body_spawn_system` is one long nested function. The slot tree could be data: a table of parent,
  anchor, offset and part, walked once.
- Only players get gear. Npcs have the empty slots.
- Body positions are bytes, and the body is capped at 256 a side (`max_model_length`).
