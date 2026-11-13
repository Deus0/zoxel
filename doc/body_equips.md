# Body and Equipment

How a character's body is built from parts, and how gear (hats, shirts) is worn on it.

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
   │  └─ thighs → calves → feet
   └─ shoulder slots (anchor left / right) → shoulders
      └─ biceps → forearms → hands
```

## Entities

| Entity | What it is | Made in |
|---|---|---|
| Realm part item | A body part kind (Chest, Head...), `BodyItem` `BodyPart`, `SlotType`, a model and icon | `bodys_realm_spawn_system`, from blueprints |
| Realm gear item | A gear kind, `EquipItem`, a `Hat` or `Shirt` tag | `equips_realm_spawn_system` (hats from vox files, the shirt from a blueprint) |
| User item | One character's copy of a realm item (an instance of it) | `spawn_user_item` |
| Slot | Where an item attaches: `SlotAnchor`, `PartOffset`, then `PartPosition` and `PartSize` once built | `spawn_body_slot`, `spawn_equip_slot` (both `spawn_attach_slot`) |
| Body vox | The character's merged voxels (`VoxLink`), `CombineList` of the parts' voxes | the character |
| Bones | One a part slot, sized and placed from the slot | `character_bone_spawn_system` |

## Making the realm's parts

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
   - It also makes the empty gear slots: the shirt over the chest, the hat on the head.
2. `character_player_equips_system` (`src/equipment/sys/character.c`), players only, when generation ends:
   - `equip_realm_item` puts the realm's first `Hat` in the `HatSlot` and its first `Shirt` in the `ShirtSlot`.
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

- **Gear slots** are attach slots tagged `EquipSlot`, plus what they hold (`HatSlot`, `ShirtSlot`).
- The equipment menu (`spawn_player_menu_equipment`) lists every `EquipSlot` on the body.
  - Dropping an item there only checks it is an `EquipItem`.
  - A change sets `BodyDirty`, and the body rebuilds.
- **Hats** are vox files (`hatty`, `cavalier_hat`, `top_hat`), cloned into the realm (`spawn_realm_model_item_filename`).
- **Shirts** are generated from a blueprint (`spawn_blueprint_shirt`, `src/parts/ins/shirt.c`), made of box nodes:
  - A torso box `margin` wider than the chest each side, as tall as it, with a neck hole cut from its top.
  - Sleeves `sleeve` voxels out past the torso over the shoulders (0 for none).
  - Trim in a second colour: hem, collar, cuffs and buttons.
  - At the chest's depth, so the voxels line up. Their colours come from the realm's seed.
  - The realm has two (`spawn_realm_shirt`): a boxy **Shirt** (margin 2, sleeves 4), which the player wears, and
    a tight sleeveless **Vest** (margin 1), which goes in the inventory with the second hat to swap in.
  - They sit over the chest (anchor over). Clothes merge after their sibling slots, so the sleeves cover the shoulders.
  - Their sleeves move with the arms: a vertex in a shoulder bone's box takes that bone.

## Adding gear

1. Make its model.
   - A blueprint like `spawn_blueprint_shirt`, then `spawn_realm_item_equip_blueprint`.
   - Or a vox file, then `spawn_realm_model_item_filename`.
2. Tag what it is (`Shirt`).
3. Add its slot where its parent slot is made (`character_body_spawn_system`), with an anchor and a tag (`ShirtSlot`).
   - Clothes use `body_anchor_over`.
4. To give it to players, call `equip_realm_item` with the slot and item tags.
5. Name its `SlotType` in `slot_names` (tooltips).
6. Test it like `tst shirt`.

## Tests

- `parts`: every realm part item's model has voxels, and its icon has pixels.
- `shirt` (in game):
  - the player wears the realm's shirt, over the chest: wider, from its bottom
  - its colours are in the body
  - it has no bone of its own
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
