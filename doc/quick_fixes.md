# Quick Fixes
- Remember to fix these properly later

-x UI 3Ds Lagging behind character 
    - the ordering was the problem
    - put trail_system, billboard_system, transform3_children_system onto main thread to temporary fix
    
-x Body lagging behind camera
    - make sure phases there.. seperate timings
    - write a simple cube append test
        - cube will move forward, camera will ride i
    - GOT IT: its actually todo with parenting
    - when another humanoid exists, it will grab the wrong bones
    - EcsParent vs EcsChildOf
    
- I made EcsChildOf default for now
    - fixes body lagging behind camera
    - fixed npc stats
    - due to difference in prefab spawning
