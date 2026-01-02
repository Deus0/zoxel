#include "brain.c"
#include "neuron.c"
#include "connection.c"
entity prefab_neuron;
entity prefab_connection;
entity prefab_brain;

void spawn_prefabs_neurals(ecs *world) {
    prefab_neuron = spawn_prefab_neuron(world);
    prefab_connection = spawn_prefab_connection(world);
    prefab_brain = spawn_prefab_brain(world);
}
