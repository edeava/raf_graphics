#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

out vec3 pass_normal;
out vec3 pass_world_position;
out vec2 pass_uv;


uniform mat4 uni_M;
uniform mat4 uni_V;
uniform mat4 uni_P;


uniform sampler2D heightmap;

void main()
{
	pass_uv = uv;
	vec4 world_position = uni_M * vec4(position + vec3(0.0, texture(heightmap, uv).r * 2, 0.0), 1.0);	
	pass_world_position = world_position.xyz;
	
	gl_Position = uni_P * uni_V * world_position;
	
	pass_normal = (uni_M * vec4(normal, 0.0)).xyz;
    
	
}