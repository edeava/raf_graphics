#version 330

in vec3 pass_normal;
in vec3 pass_world_position;
in vec2 pass_uv;


out vec4 final_colour;



uniform sampler2D grass;
uniform sampler2D snow;
uniform sampler2D rock;

uniform vec3 uni_light_colour;
uniform vec3 uni_light_direction;
uniform vec3 uni_camera_position;

float brightness(vec3 pix)
{
	return (pix.r + pix.g + pix.b) / 3.0;
}

void main()
{

	vec3 ambient = vec3(0.2);
	vec3 view_vector = normalize(pass_world_position - uni_camera_position);
	vec3 neg_view_vector = view_vector * (-1);
	vec3 normalized_normal = normalize(pass_normal);
	vec3 reflected_light = reflect(normalize(uni_light_direction), normalized_normal);
	
	float specular_factor = clamp(dot(reflected_light, neg_view_vector), 0, 1);
	specular_factor = pow(specular_factor, 5.0);
	vec3 specular_colour = uni_light_colour * specular_factor;
	
	float light_factor = clamp(dot(normalize(pass_normal), normalize(-uni_light_direction)), 0, 1);
    vec3 diffuse_colour = uni_light_colour * light_factor;
	
	vec3 normal = normalize(pass_normal);
	vec4 triangleColor = vec4(0.0);

	if (pass_world_position.y < 0.5f){
		triangleColor = texture(grass, pass_uv);
	} else if (pass_world_position.y <= 0.7f){
		vec4 triangleColorCurr = texture(grass, pass_uv);
		vec4 triangleColorNext = texture(rock, pass_uv);

		float level = 0.2f;
		float factorNext = (pass_world_position.y - 0.5f) / level;
		float factorCurr = 1.0f - factorNext;

		triangleColor = triangleColorCurr * factorCurr + triangleColorNext * factorNext;
	} else if (pass_world_position.y < 1.0f){
		triangleColor = texture(rock, pass_uv);
	} else if (pass_world_position.y <= 1.2f){
		vec4 triangleColorCurr = texture(rock, pass_uv);
		vec4 triangleColorNext = texture(snow, pass_uv);

		float level = 0.2f;
		float factorNext = (pass_world_position.y - 1.0f) / level;
		float factorCurr = 1.0f - factorNext;

		triangleColor = triangleColorCurr * factorCurr + triangleColorNext * factorNext;
	} else {
		triangleColor = texture(snow, pass_uv);
	}


	final_colour = vec4(diffuse_colour + specular_colour, 1.0) * triangleColor;
	
}