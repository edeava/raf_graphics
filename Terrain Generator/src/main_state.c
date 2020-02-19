#include <main_state.h>
#include <glad/glad.h>
#include <math.h>


#include <rafgl.h>

#include <game_constants.h>

#define DIM 20
#define FIRST vec3(10, 0.0f, 10)
#define SECOND vec3(-10.0f, 0.0f, 10.0f)
#define THIRD vec3(-10.0f, 0.0f, -10.0f)
#define FOURTH vec3(10.0f, 0.0f, -10.0f)

rafgl_texture_t heightmap, grass, snow, rock;


rafgl_raster_t raster_hm;

void load_tex(rafgl_texture_t *tex, const char *path)
{
    rafgl_raster_t raster;
    rafgl_raster_load_from_image(&raster, path);
    rafgl_texture_init(tex);
    rafgl_texture_load_from_raster(tex, &raster);

    glBindTexture(GL_TEXTURE_2D, tex->tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

}


typedef struct
{
    GLuint program_id;
    GLuint uni_M, uni_V, uni_P;

    GLuint uni_light_colour;
    GLuint uni_light_direction;
    GLuint uni_camera_position;


    GLuint special_unis[16];

    void (*tex_fun)(GLuint);

} shader_program_t;

typedef struct ravan{
    rafgl_meshPUN_t p1, p2, p3, p4;
} plane_t;

shader_program_t terrain_shader;

void terrain_tex_fun(GLuint prog_id)
{
    glUniform1i(glGetUniformLocation(prog_id, "heightmap"), 0);
    glUniform1i(glGetUniformLocation(prog_id, "grass"), 1);
    glUniform1i(glGetUniformLocation(prog_id, "rock"), 2);
    glUniform1i(glGetUniformLocation(prog_id, "snow"), 3);

}

void load_terrain_shader()
{
    GLuint id = terrain_shader.program_id = rafgl_program_create_from_name("terrain_shader");
    terrain_shader.tex_fun = terrain_tex_fun;
    terrain_shader.uni_M = glGetUniformLocation(id, "uni_M");
    terrain_shader.uni_V = glGetUniformLocation(id, "uni_V");
    terrain_shader.uni_P = glGetUniformLocation(id, "uni_P");

    terrain_shader.uni_light_colour = glGetUniformLocation(id, "uni_light_colour");
    terrain_shader.uni_light_direction = glGetUniformLocation(id, "uni_light_direction");
    terrain_shader.uni_camera_position = glGetUniformLocation(id, "uni_camera_position");

}

void bind_shader(shader_program_t *sp)
{
    if(sp != NULL)
    {
        glUseProgram(sp->program_id);
        sp->tex_fun(sp->program_id);
    }
    else
    {
        glUseProgram(0);
    }
}


vec3_t light_direction;
vec3_t light_colour;
int lod0 = 128, lod1 = 64, lod2 = 32, lod3 = 16;
plane_t plane[4];
vec3_t quads[4];

void main_state_init(GLFWwindow *window, void *args, int width, int height)
{
    quads[0] = FIRST;
    quads[1] = SECOND;
    quads[2] = THIRD;
    quads[3] = FOURTH;

    light_direction = v3_norm(vec3(-1, -1, -1));
    light_colour = vec3(1.0f, 0.9f, 0.7f);

    for(int i = 0; i < 4; i++){
        rafgl_meshPUN_init(&plane[i].p1);
        rafgl_meshPUN_load_plane_offset(&plane[i].p1, DIM, DIM, lod0, lod0, quads[i]);

        rafgl_meshPUN_init(&plane[i].p2);
        rafgl_meshPUN_load_plane_offset(&plane[i].p2, DIM, DIM, lod1, lod1, quads[i]);

        rafgl_meshPUN_init(&plane[i].p3);
        rafgl_meshPUN_load_plane_offset(&plane[i].p3, DIM, DIM, lod2, lod2, quads[i]);

        rafgl_meshPUN_init(&plane[i].p4);
        rafgl_meshPUN_load_plane_offset(&plane[i].p4, DIM, DIM, lod3, lod3, quads[i]);
    }

    load_terrain_shader();

    load_tex(&heightmap, "res/images/height1.png");
    load_tex(&grass, "res/images/grass.jpg");
    load_tex(&rock, "res/images/rock.jpg");
    load_tex(&snow, "res/images/snow.jpg");

}

mat4_t model, view, projection, view_projection;

float fov = 75.0f;

vec3_t camera_position = vec3m(10.0f, 2.0f, 20.0f);
vec3_t camera_up = vec3m(0.0f, 1.0f, 0.0f);
vec3_t aim_dir = vec3m(0.0f, 0.0f, -1.0f);

float camera_angle = -M_PIf * 0.5f;
float angle_speed = 0.2f * M_PIf;
float move_speed = 2.4f;

float hoffset = 0;
int p = -1;

int rotate = 0;
float model_angle = 0.0f;

void v3show(vec3_t v)
{
    printf("(%.2f %.2f %.2f)\n", v.x, v.y, v.z);
}

float v3distance(vec3_t p, vec3_t q){
    return sqrt((p.x - q.x) * (p.x - q.x) + (p.y - q.y) * (p.y - q.y) + (p.z - q.z) * (p.z - q.z)) + fov / 10;
}

float time = 0.0f;
int reshow_cursor_flag = 0;
int last_lmb = 0;

float sensitivity = 3.0f;

float visibility_factor = -2.0f;
float turn = 1.0f;

void main_state_update(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args)
{
    printf("%f\n", v3distance(camera_position, FIRST));
    time += delta_time;
    model_angle += delta_time * rotate;


    if(!game_data->keys_down[RAFGL_KEY_LEFT_SHIFT])
    {
        angle_speed = 0.2f * M_PIf;
        move_speed = 2.4f;
        sensitivity = 1.0f;
    }
    else
    {
        angle_speed = 5 * 0.2f * M_PIf;
        move_speed = 5 * 2.4f;
        sensitivity = 10.0f;
    }

    if(game_data->is_lmb_down)
    {

        if(reshow_cursor_flag == 0)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }

        float ydelta = game_data->mouse_pos_y - game_data->raster_height / 2;
        float xdelta = game_data->mouse_pos_x - game_data->raster_width / 2;

        if(!last_lmb)
        {
            ydelta = 0;
            xdelta = 0;
        }

        hoffset -= sensitivity * ydelta / game_data->raster_height;
        camera_angle += sensitivity * xdelta / game_data->raster_width;

        glfwSetCursorPos(window, game_data->raster_width / 2, game_data->raster_height / 2);
        reshow_cursor_flag = 1;
    }
    else if(reshow_cursor_flag)
    {
        reshow_cursor_flag = 0;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    last_lmb = game_data->is_lmb_down;

    aim_dir = v3_norm(vec3(cosf(camera_angle), hoffset, sinf(camera_angle)));

    if(game_data->keys_down['W']) camera_position = v3_add(camera_position, v3_muls(aim_dir, move_speed * delta_time));
    if(game_data->keys_down['S']) camera_position = v3_add(camera_position, v3_muls(aim_dir, -move_speed * delta_time));

    vec3_t right = v3_cross(aim_dir, vec3(0.0f, 1.0f, 0.0f));
    if(game_data->keys_down['D']) camera_position = v3_add(camera_position, v3_muls(right, move_speed * delta_time));
    if(game_data->keys_down['A']) camera_position = v3_add(camera_position, v3_muls(right, -move_speed * delta_time));
    if(game_data->keys_down['Q']) fov = fov > 105 ? fov : fov + 10;
    if(game_data->keys_down['E']) fov = fov < 45 ? fov : fov - 10;
    if(game_data->keys_down['P']) p *= -1;


    if(game_data->keys_down[RAFGL_KEY_ESCAPE]) glfwSetWindowShouldClose(window, GLFW_TRUE);

    if(game_data->keys_down[RAFGL_KEY_SPACE]) camera_position.y += 1.0f * delta_time * move_speed;
    if(game_data->keys_down[RAFGL_KEY_LEFT_CONTROL]) camera_position.y -= 1.0f * delta_time * move_speed;


    int i;

    float aspect = ((float)(game_data->raster_width)) / game_data->raster_height;    projection = m4_perspective(fov, aspect, 0.1f, 100.0f);

    if(!game_data->keys_down['T'])
    {
        view = m4_look_at(camera_position, v3_add(camera_position, aim_dir), camera_up);
    }
    else
    {
        view = m4_look_at(camera_position, vec3(0.0f, 0.0f, 0.0f), camera_up);
    }

    model = m4_identity();
    model = m4_rotation_y(model_angle);
    model = m4_mul(model, m4_translation(vec3(0.0f, sinf(model_angle) * 0.45, 0.0f)));

    view_projection = m4_mul(projection, view);
}


void load_vec3(GLuint location, vec3_t v)
{
    glUniform3f(location, v.x, v.y, v.z);
}

void load_mat4(GLuint location, mat4_t *m)
{
    glUniformMatrix4fv(location, 1, GL_FALSE, &(m->m00));
}

void main_state_render(GLFWwindow *window, void *args)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    bind_shader(&terrain_shader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightmap.tex_id);

    if(p == 1)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, grass.tex_id);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, rock.tex_id);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, snow.tex_id);


    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    load_vec3(terrain_shader.uni_light_colour, light_colour);
    load_vec3(terrain_shader.uni_light_direction, light_direction);
    load_vec3(terrain_shader.uni_camera_position, camera_position);

    mat4_t identity = m4_identity();
    load_mat4(terrain_shader.uni_M, &identity);
    load_mat4(terrain_shader.uni_V, &view);
    load_mat4(terrain_shader.uni_P, &projection);

    for(int i = 0; i < 4; i++){
        if(v3distance(camera_position, quads[i]) < 22.5){
            glBindVertexArray(plane[i].p1.vao_id);
            glDrawArrays(GL_TRIANGLES, 0, plane[i].p1.vertex_count);
        }else if(v3distance(camera_position, quads[i]) < 32.5){
            glBindVertexArray(plane[i].p2.vao_id);
            glDrawArrays(GL_TRIANGLES, 0, plane[i].p2.vertex_count);
        }else if(v3distance(camera_position, quads[i]) < 39.5){
            glBindVertexArray(plane[i].p3.vao_id);
            glDrawArrays(GL_TRIANGLES, 0, plane[i].p3.vertex_count);
        }else{
            glBindVertexArray(plane[i].p4.vao_id);
            glDrawArrays(GL_TRIANGLES, 0, plane[i].p4.vertex_count);
        }
    }

    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}


void main_state_cleanup(GLFWwindow *window, void *args)
{

}
