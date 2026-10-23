#version 450

layout(location = 0) in vec2 in_uv;

layout(set = 2, binding = 0) uniform sampler2D sprite;

layout(location = 0) out vec4 out_color;

void main() {
	vec4 color = texture(sprite, in_uv);

	out_color = color;
}

