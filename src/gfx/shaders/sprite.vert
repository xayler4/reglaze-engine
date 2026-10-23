#version 450

layout (set = 1, binding = 0) uniform CameraInfo {
    mat4 projection_view;
} camera;

layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in mat4 in_model_matrix; 

layout (location = 0) out vec2 out_uv;

void main() {
    out_uv = in_uv;

    vec4 world_pos = in_model_matrix * vec4(in_position, 0.0, 1.0);
    
    gl_Position = camera.projection_view * world_pos;
}
