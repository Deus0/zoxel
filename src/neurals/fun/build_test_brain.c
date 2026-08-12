void build_brain_test(ecs *world, const entity e, const int2 size) {
    int connections_per_layer = (size.y) + (size.y - 1) * 2;
    int neurons_per_layer = size.y;
    int total_connections = connections_per_layer * (size.x - 1);
    int children_length = neurons_per_layer * size.x + total_connections;
    Children *children = zox_get_mut(e, Children);
    resize_memory_component(Children, children, entity, children_length);
    BrainInputs *brain_inputs = zox_get_mut(e, BrainInputs);
    resize_memory_component(BrainInputs, brain_inputs, entity, neurons_per_layer);
    BrainOutputs *brain_outputs = zox_get_mut(e, BrainOutputs);
    resize_memory_component(BrainOutputs, brain_outputs, entity, neurons_per_layer);
    BrainLinks *brain_links = zox_get_mut(e, BrainLinks);
    resize_memory_component(BrainLinks, brain_links, entity, total_connections);
    int k = 0; // total index for child
    int l = 0; // link array index
    int neuron_start_index = 0;
    for (int x = 0; x < size.x; x++) {
        int neuron_index_next = k;
        for (int y = 0; y < size.y; y++) {
            // const float weight = (rand() % 101) * 0.01f;
            float2 position = (float2) { x, y };
            position = calculate_neuron_position(position, size.x, size.y);
            const entity e2 = spawn_neuron(world, prefab_neuron, e, position);
            if (x == 0) {
                zox_add_tag(e2, InputNeuron);
                brain_inputs->value[y] = e2;
            }
            else if (x == size.x - 1) {
                zox_add_tag(e2, OutputNeuron);
                brain_outputs->value[y] = e2;
            }
            children->value[k++] = e2;
        }
        if (x == 0) {
            neuron_start_index = neuron_index_next;
            continue;
        }
        for (int y = 0; y < size.y; y++) {
            const float weight = (rand() % 101) * 0.01f;
            const entity a = children->value[neuron_start_index + y];
            const entity b = children->value[neuron_index_next + y];
            const entity e2 = spawn_connection(world, prefab_connection, e, a, b, weight);
            children->value[k++] = e2;
            brain_links->value[l++] = e2;
        }
        for (int y = 1; y < size.y; y++) {
            const float weight = (rand() % 101) * 0.01f;
            const entity a = children->value[neuron_start_index + y];
            const entity b = children->value[neuron_index_next + y - 1];
            const entity e2 = spawn_connection(world, prefab_connection, e, a, b, weight);
            children->value[k++] = e2;
            brain_links->value[l++] = e2;
        }
        for (int y = 0; y < size.y - 1; y++) {
            const float weight = (rand() % 101) * 0.01f;
            const entity a = children->value[neuron_start_index + y];
            const entity b = children->value[neuron_index_next + y + 1];
            const entity e2 = spawn_connection(world, prefab_connection, e, a, b, weight);
            children->value[k++] = e2;
            brain_links->value[l++] = e2;
        }
        neuron_start_index = neuron_index_next;
    }
}

void spawn_test_brain(ecs *world) {
    const entity brain = spawn_brain(world, prefab_brain);
    build_brain_test(world, brain, (int2) { brain_layers, brain_texture_size.x * brain_texture_size.y });
    local_brain = brain;
}
