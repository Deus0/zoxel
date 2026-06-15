general_fun_array_d* update_functions;

void initialize_update_loop() {
    update_functions = create_general_fun_array_d(initial_dynamic_array_size);
}

void dispose_update_loop() {
    dispose_general_fun_array_d(update_functions);
}

void add_to_update_loop(void (*event)(ecs *)) {
    general_fun fun_event = (general_fun) { event };
    general_fun_array_d_add(update_functions, fun_event);
}

void run_update_loop(ecs *world) {
    for (size_t i = 0; i < update_functions->size; i++) {
        if (update_functions->data[i].value != NULL) {
            (*update_functions->data[i].value)(world);
        }
    }
}