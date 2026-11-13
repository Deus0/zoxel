# New
- Part of the [todo](todo.md), done items go in [done.md](done.md)
- New modules and features

## Quadpeds
- add a second skeleton type, thats a four walker, and a wolf npc, so itd need new body parts etc
	- this is an expansion of current body parts and animations
	- we will need a head generated differently to current humanoid ones
	- we should polish the blueprint system too that body parts use
	- it also needs a tail, later we can add more bones per part, but for now itll still use the same simple methods

## Turrets
- Turrets are items
- Can place like a minivox
- They have seperate parts, so different entity
	- base vox model
	- cannon model
- aim cannon at targets
- instanced rendering
- uses projectiles to hit nearby targets

## Crafting
- Fix Crafting and use Slots for them
- Output Craft Slot, pickup only
- Pickup item to change after any of the other slots change, using recipe data to match

## Pets
- Spawn mr penguin on screen
- Give the ability to change his hat
- Make a penguin module
- Adds a penguin to taskbar
- Adds a penguin render scene and 3 buttons to change his hat
- remove any use of user macros from user data
	- just reuse their prefabs and add tags
- use functions like spawn_window, or spawn_user_grid inside those uis
- Spawn label top left above stats - show block / npc selected
- Generate a mouse texture - arrow
- Maximize a window - button on map header
- Resize window grabber at corner
- move sand/wood/stone into biome blocks

## genetics
- Add dna, array of bytes, per npc
- only for unique npcs
- dna linked in their body parts too
- its used as a seed, so all parts use it
- population is at a region level, it has data for the entire coutn
- npcs can breed and cross dna for their kids

## affection
- npcs can have affection or hostility for others
- this can effect clan alignment
- it will also increase chance for breeding

## towns
- villager tags for npcs in towns
- they can have roles, like mayor, 'jobs' module
