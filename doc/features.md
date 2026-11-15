# Features
- What each module does now, by module. Part of the [todo](todo.md) ("a features.md with all features by module").
- A section a module: what it does, its components, its systems, and what's missing next to a game engine.
- Drafted by the local ai a few modules a run, reviewed after: sections marked (draft) aren't checked yet.

## sky (draft)
- Does: generates and manages a dynamic sky with drifting clouds above the terrain, creating an immersive environment
- Components: Cloud - tags entities as clouds; CloudCentre - stores the center point for cloud wrapping around the camera
- Systems: clouds_spawn_system - spawns clouds for each realm's camera position; cloud_drift_system - moves clouds with wind and wraps them around the camera
- Missing: atmospheric effects like sun flares, starfield, or dynamic weather transitions

## weathers (draft)
- Does: manages skybox colors, blending between different weather states, and fog effects in the game environment
- Components: Weather - tags entities as weather systems; Skybox - tags entities as skyboxes; SkyColorFrom - stores the starting color for blending; SkyColorTarget - stores the target color for blending; SkyBlend - stores the blend progress (0 to 1); SkyBlendLink - links to a tween animation for blending
- Systems: skybox_restore_system - restores skybox GPU materials and colors; skybox_set_time_system - updates time-based properties in skybox shaders; sky_blend_system - blends between sky colors over time using the SkyBlend value
- Missing: precipitation effects like rain or snow, wind effects, or complex atmospheric scattering

## pickups (draft)
- Does: handles item collection, spawning, and animation for collectible items in the game world
- Components: Pickup - tags entities as collectible items; PickUpperer - tags entities that can pick up items; PickedUp - tracks the pickup state of an item; PickupPop - tags items that are currently popping after being dropped; PickupPopStart - stores the start position for a pop animation; PickupPopEnd - stores the end position for a pop animation; PickupPopHeight - stores the height of the pop arc; PickupPopTime - stores when the pop animation started
- Systems: pickup_sound_system - plays pickup sounds when items are collected; pickup_pop_system - animates dropped items popping to nearby positions
- Missing: item stacking and inventory management, visual effects for item collection, or particle effects for pickups

## particles (draft)
- Does: manages 2D and 3D particle effects including emitters, lifetimes, motion, and rendering of particles in the game world
- Components: Particle - tags entities as 2D particles; ParticlesEmitter - tags entities as particle emitters; ParticleEmitRate - stores the rate at which particles are emitted; Particle2D - tags entities as 2D particles; Particle3D - tags entities as 3D particles; Particle3DEmitter - tags entities as 3D particle emitters
- Systems: particle3d_emit_system - emits 3D particles from emitters with specified properties; particle3d_render_system - renders 3D particles using instancing or point rendering techniques
- Missing: advanced particle effects like explosion or fire animations, texture-based particle rendering, or physics-based particle interactions

## markers (draft)
- Does: handles overhead icons, labels, and visual markers for entities in the game world
- Components: Marker - tags entities as markers; MarkerLink - links marker entities to their parent entities
- Systems: 
- Missing: advanced marker types like quest indicators, combat effects, or interactive UI elements for markers

## maps (draft)
- Does: manages world maps, minimaps, navigation, and region-based rendering in the game environment
- Components: MenuMap - tags entities as map UI elements; Map - tags entities as map views; Minimap - tags entities as minimap displays; MapArrow - tags entities as directional arrows on maps; MapZoom - stores zoom level for maps; MapPosition - stores position coordinates for maps; MapPiecePosition - stores position of map pieces; MapView - tags entities as map view elements; MapBlocksPerTexel - stores blocks per texel for map rendering; MapDrawnAt - stores where map was last drawn; MapDrawnVersion - stores version of map when drawn; MapDrawnWhole - indicates if whole map was drawn; MapDrawnTime - stores time when map was last drawn
- Systems: map_initialize_system - initializes map elements with proper settings; map_position_system - updates map positions based on player movement; map_view_system - renders map views using texture data and handles updating of map pieces; map_arrow_system - updates arrow directions for map markers based on player rotation
- Missing: dynamic map loading, terrain-based navigation, or advanced minimap features like waypoints or player tracking

## quests (draft)
- Does: manages quest objectives, progress tracking, rewards, and quest state within the game world
- Components: Quest - tags entities as quests; QuestDoing - tags entities as currently active quests; QuestGiving - tags entities that can give quests; QuestGiver - tags entities that are quest givers; QuestGiven - tags entities as given quests; QuestDone - tags entities as completed quests; QuestHandedin - tags entities as handed in quests; SlayQuest - tags entities as slay-type quests; NodeActionQuest - tags nodes for quest actions; NodeGiveQuest - tags nodes for giving quests; NodeHandinQuest - tags nodes for handing in quests; NodeHasQuest - tags nodes that have quests; NodeHasQuestStarted - tags nodes that have started quests; NodeHasQuestDone - tags nodes that have done quests; NodeHasQuestHandedin - tags nodes that have handed in quests; QuestValue - stores the current quest value; QuestTarget - stores the target quest value; QuestLink - links entities to their related quests
- Systems: character_player_quests_system - assigns initial quests to player characters; marker_spawn_system - spawns quest markers above quest givers
- Missing: complex quest chains with interdependencies, quest conditions based on game events, or visual quest tracking UI elements

## dialogues (draft)
- Does: handles conversations, choices, responses, and dialogue events in the game
- Components: Dialogue - tags entities as dialogues; SpeechNode - tags entities as speech nodes; DialogueOptionNode - tags entities as dialogue option nodes; DialogueProcess - tags entities as dialogue processes; DialogueUI - tags entities as dialogue UI elements; DialogueLabel - tags entities as dialogue labels; DialogueButton - tags entities as dialogue buttons; Greetings - tags entities as greeting dialogues; QuestDialogue - tags entities as quest dialogues; DialogueText - stores the dialogue text; SpeakerLinks - links dialogue processes to speakers; SpeakerLink - links dialogue text to its speaker; VoiceIndex - stores the current voice index for text animation
- Systems: dialogue_speech_system - manages speech display and animation in dialogue UIs; animate_text_end_system - handles text animation end events; character_dialogue_system - assigns dialogues to characters; dialogue_sound_system - plays dialogue sounds using voice synthesis
- Missing: advanced dialogue trees with branching paths, dynamic dialogue generation based on game state, or interactive dialogue options that affect story outcomes

## crafting (draft)
- Does: manages recipes, materials, crafting stations, and production processes in the game
- Components: Craftspace - tags entities as crafting spaces; Recipe - tags entities as recipes; MenuCrafting - tags entities as crafting menu UI elements
- Systems: none
- Missing: crafting recipe unlocking mechanics, inventory management for materials, or visual feedback for crafting success/failure

