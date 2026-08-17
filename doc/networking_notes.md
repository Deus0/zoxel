# Networking Notes

## Enable Ghosts
- Server will recive position updates from clients
- It will then resend those back to any other clients that use the same seeds
- you will see instanced Cubes, barely transparent (fade in and out) at their locations
- can set ghost ip and toggle in settings

## We can make connections entities under realm that link to players
- This lets us not care if theyre a room or client much

entity connection = ecs_new(world);
zox_add(connection, SocketLink);
zox_add(connection, NetConnection);
zox_link(world, connection, NetPlayer, player);

- Add a packet queue that entities can process
