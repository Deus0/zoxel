# Playtest Results
* Put these into todo now section

New
- right clicking / alternative action button:
- cant sleep in bed
- cant rotate / open doors
- placement gizmo for multiple block placement should show more
- The raycasting doesnt work on the multi block
- you can also place new blocks over it, so placement isnt checking it
    - its meant to be set in octree, clearly it wasnt
- physics not working on multiblock - extra block
- it should despawn when you destroy whats under them
    - like grass, bed, door etc
    - needs a tag for this to check when its destroyed
- might need resize character body (part) dimensions to fit the 2 block tall, 1 wide
    - we can add taller characters too <= 3, 2 wide, etc with bigger doors later for other races
- spawn doors and beds in houses


Last One
- the lighting issues in house is from lights issues
    - tested with light gizmos
    - the lights in half the house is all bright, suggesting is generated before the house was placed
    - Or didnt regenerate after?
- player feels no direction on start
    - have an npc target you, and come up to you, and talk to you, when you first spawn
    - you will have a tag for them to find you if they see you
    - they give you a town starter quest
- come bacck with cookies, still no cokie collector quest
- player still not facing npc when walking to them
- npcs shouldnt jump randomly when you are talking *unless theyre hyper*
- minimap flashes black when updating lods
- cant hear footsteps well unless very loud, maybe balance is off
    - can turn down the music a little
- healthbar isnt first still in the panel, soul ended up last
    - soul needs to be first... lol
