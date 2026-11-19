// a table's new entry (zeroed), its id: a freed one again, else the next
uint vk_table_add(vk_table* table) {
    uint id;
    if (table->free_count) {
        id = table->free[--table->free_count];
    } else {
        id = ++table->count;
        if (id >= table->capacity) {
            table->capacity = table->capacity ? table->capacity * 2 : 64;
            table->data = realloc(table->data, table->capacity * table->size);
            table->free = realloc(table->free, table->capacity * sizeof(uint));
        }
    }
    memset(table->data + id * table->size, 0, table->size);
    return id;
}

void vk_table_remove(vk_table* table, uint id) {
    table->free[table->free_count++] = id;
}

static inline void* vk_table_get(vk_table* table, uint id) {
    return id && id <= table->count ? table->data + id * table->size : NULL;
}

void vk_table_dispose(vk_table* table) {
    free(table->data);
    free(table->free);
    *table = (vk_table) { .size = table->size };
}

static inline vk_buffer* vk_buffer_of(uint id) {
    vk_buffer* buffer = vk_table_get(&vk_buffers, id);
    return buffer && buffer->live ? buffer : NULL;
}

static inline vk_texture* vk_texture_of(uint id) {
    vk_texture* texture = vk_table_get(&vk_textures, id);
    return texture && texture->live ? texture : NULL;
}

static inline vk_shader* vk_shader_of(uint id) {
    vk_shader* shader = vk_table_get(&vk_shaders, id);
    return shader && shader->live ? shader : NULL;
}

static inline vk_program* vk_program_of(uint id) {
    vk_program* program = vk_table_get(&vk_programs, id);
    return program && program->live ? program : NULL;
}
