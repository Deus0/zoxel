#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int is_declared(const char* source, const char* type, const char* var) {
    const char* p = source;
    size_t type_len = strlen(type);
    size_t var_len  = strlen(var);

    while ((p = strstr(p, type)) != NULL) {
        const char* after_type = p + type_len;

        // Skip whitespace
        while (*after_type && isspace(*after_type)) after_type++;

        // Check if variable name follows
        if (strncmp(after_type, var, var_len) == 0) {
            const char next_char = after_type[var_len];
            // Must be end of name: space, ;, =, [, etc.
            if (isspace(next_char) || next_char == ';' || next_char == '=' || next_char == '[')
                return 1; // found
        }

        p += type_len;
    }

    return 0; // not found
}

// TODO: Work on this
char* convert_to_gles2_shader(const char* source) {
    if (!source) {
        return NULL;
    }
    size_t len = strlen(source);
    size_t new_len = len * 3 + 128;
    char* buffer = malloc(new_len);
    if (!buffer) return NULL;

    size_t offset = 0;
    const char* p = source;
    // Copy #version line first (if exists)
    if (strncmp(p, "#version", 8) == 0) {
        while (*p && *p != '\n') buffer[offset++] = *p++;
        if (*p == '\n') buffer[offset++] = *p++;
    }
    // Inject only if used and not declared
    if (strstr(source, "vertex_position") && !is_declared(source, "vec2", "vertex_position")) {
        offset += snprintf(buffer + offset, new_len - offset, "attribute vec2 vertex_position;\n");
    }
    if (strstr(source, "vertex_uv") && !is_declared(source, "vec2", "vertex_uv")) {
        offset += snprintf(buffer + offset, new_len - offset, "attribute vec2 vertex_uv;\n");
    }
    // Main replacement loop
    while (*p) {
        if (isspace(*p)) { buffer[offset++] = *p++; continue; }
        // Remove layout(location=...) entirely
        if (strncmp(p, "layout(location", 15) == 0) {
            while (*p && *p != ';') p++;
            if (*p == ';') p++;
        }
        // Fragment shader: 'out lowp vec4 color;' -> 'precision lowp float;'
        else if (strncmp(p, "out lowp vec4 color;", 20) == 0) {
            const char* rep = "precision lowp float;";
            size_t rlen = strlen(rep);
            memcpy(buffer + offset, rep, rlen);
            offset += rlen; p += 20;
        }
        // Both Shader: 'attribute' -> 'varying'
        else if (strncmp(p, "attribute ", 10) == 0) {
            const char* rep = "varying ";
            size_t rlen = strlen(rep);
            memcpy(buffer + offset, rep, rlen);
            offset += rlen; p += 10;
        }
        // Vertex shader: 'out' -> 'varying'
        else if (strncmp(p, "out ", 4) == 0) {
            const char* rep = "varying ";
            size_t rlen = strlen(rep);
            memcpy(buffer + offset, rep, rlen);
            offset += rlen;
            p += 4;
        }
        // Fragment shader: 'in' -> 'varying'
        else if (strncmp(p, "in ", 3) == 0) {
            const char* rep = "varying ";
            size_t rlen = strlen(rep);
            memcpy(buffer + offset, rep, rlen);
            offset += rlen;
            p += 3;
        }
        // Fragment shader: 'color =' -> 'gl_FragColor ='
        else if (strncmp(p, "color =", 7) == 0) {
            const char* rep = "gl_FragColor =";
            size_t rlen = strlen(rep);
            memcpy(buffer + offset, rep, rlen);
            offset += rlen; p += 7;
        }
        else if (strncmp(p, "color.", 6) == 0) {
            const char* rep = "gl_FragColor.";
            size_t rlen = strlen(rep);
            memcpy(buffer + offset, rep, rlen);
            offset += rlen; p += 6;
        }
        else if (strncmp(p, "color,", 6) == 0) {
            const char* rep = "gl_FragColor,";
            size_t rlen = strlen(rep);
            memcpy(buffer + offset, rep, rlen);
            offset += rlen; p += 6;
        }
        else if (strncmp(p, "texture(", 8) == 0) {
            const char* rep = "texture2D(";
            size_t rlen = strlen(rep);
            memcpy(buffer + offset, rep, rlen);
            offset += rlen; p += 8;
        }
        else buffer[offset++] = *p++;
    }
    buffer[offset] = '\0';
    return buffer;
}
