#include <main_state.h>
#include <glad/glad.h>
#include <math.h>
#include <time.h>


#include <rafgl.h>

#include <game_constants.h>
#include <../weapon_constants.h>

static rafgl_raster_t doge;
static rafgl_raster_t upscaled_doge;
static rafgl_raster_t raster, raster2;
static rafgl_raster_t checker;

static rafgl_texture_t texture;

rafgl_spritesheet_t number;

static int raster_width = RASTER_WIDTH, raster_height = RASTER_HEIGHT;

static char save_file[256];
int save_file_no = 0;

/*typedef struct _particle_t
{
    float x, y, dx, dy;
    int life;


} particle_t;*/

//#define MAX_PARTICLES 1

//particle_t particles[MAX_PARTICLES];

int groundLevel[RASTER_WIDTH / CHUNK], clickable = 1, cng = 0, choosenWeapon = 0;
weapon_t bullet;
player_t player1, player2;
player_t *activePlayer, *opositePlayer;

void initGround(){
    srand(time(NULL));
    int boundry = RASTER_WIDTH / CHUNK;
    int m = rand() % 32, slope = (rand() % 32) - 16;
    printf("%d %d\n", m, slope);
    for(int i = 0; i < boundry; i++){
        if(i <= (boundry / 4))
            groundLevel[i] = 3 * raster_height / 4;
        else if(i > (boundry / 2))
            groundLevel[i] = groundLevel[boundry - i];
        else
            groundLevel[i] = /*(rand() % m) - slope +*/ groundLevel[i - 1];

        groundLevel[i] = groundLevel[i] < (raster_height / 8) ? (raster_height / 8) : groundLevel[i];
    }
    initG(groundLevel, boundry);
}

void damage(int d, int x){
    for(int i = x - d; i < x + d; i++){
        groundLevel[i] += i <= x ? i - x + d : x - i + d;
        if(i == (int)((*opositePlayer).x / CHUNK))
            (*activePlayer).points += i <= x ? i - x + d : x - i + d;
    }
    initG(groundLevel, RASTER_WIDTH / CHUNK);
}

void main_state_init(GLFWwindow *window, void *args)
{

    rafgl_raster_init(&raster, raster_width, raster_height);
    rafgl_raster_init(&raster2, raster_width, raster_height);

    initGround();

    initPlayer(&player1, 1, &raster);
    initPlayer(&player2, 2, &raster);
    activePlayer = &player1;
    opositePlayer = &player2;

    rafgl_texture_init(&texture);
    rafgl_spritesheet_init(&(number), "res/images/number.png", 10, 1);
}

int pressedR, pressedM;
float location = 0;
float selector = 0;


void main_state_update(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args)
{

    if(game_data->keys_down[RAFGL_KEY_A])
        initData('A');
    else if(game_data->keys_down[RAFGL_KEY_D])
        initData('D');
    else initData(0);

    if(game_data->keys_down[RAFGL_KEY_1]){
        choosenWeapon = 0;
    }
    if(game_data->keys_down[RAFGL_KEY_2]){
        choosenWeapon = 1;
    }
    if(game_data->keys_down[RAFGL_KEY_3]){
        choosenWeapon = 2;
    }
    if(game_data->keys_down[RAFGL_KEY_4]){
        choosenWeapon = 3;
    }

    if(game_data->is_rmb_down)
    {
        pressedR = 1;
        location = rafgl_clampf(game_data->mouse_pos_y, 0, raster_height - 1);
        selector = 1.0f * location / raster_height;
        printf("%f\n", 1 - selector);
        activePlayer->azimut = 1 - selector;
    }
    else
    {
        pressedR = 0;
    }

    if(game_data->is_mmb_down)
    {
        pressedM = 1;
        location = rafgl_clampf(game_data->mouse_pos_y, 0, raster_height - 1);
        selector = 1.0f * location / raster_height;
        printf("%f\n", 1 - selector);
        activePlayer->sCoef = 1 - selector;
    }
    else
    {
        pressedM = 0;
    }

    int i, gen = 5, radius = 10;
    float angle, speed;
    if(game_data->is_lmb_down && clickable)
    {
        clickable = 0;
        cng = cng ^ 1;

        activePlayer->weapons[choosenWeapon].weapon_fire(activePlayer);

    }

    int x, y;

    float xn, yn;

    rafgl_pixel_rgb_t sampled, sampled2, resulting, resulting2;


    for(y = 0; y < raster_height; y++)
    {
        yn = 1.0f * y / raster_height;
        for(x = 0; x < raster_width; x++)
        {
            xn = 1.0f * x / raster_width;

            sampled = pixel_at_m(raster, x, y);
            if(y >= groundLevel[x / CHUNK]){
                resulting.rgba = rafgl_RGBA(0, 255, 0, 0);
            }else
                resulting.rgba = rafgl_RGB(0, 0, 0);

            pixel_at_m(raster, x, y) = resulting;


            if((pressedR || pressedM) && rafgl_distance1D(location, y) < 3 && x > raster_width - 15)
            {
                if(pressedM){
                    pixel_at_m(raster, x, y).rgba = rafgl_RGB(0, 0, 255);
                    printNum((int)(activePlayer->sCoef * 100), (int)activePlayer->x, 40);
                }
                if(pressedR){
                    pixel_at_m(raster, x, y).rgba = rafgl_RGB(255, 0, 0);
                    printNum((int)(activePlayer->azimut * 360), (int)activePlayer->x, 40);
                }
            }

        }
    }

    activePlayer->weapons[choosenWeapon].weapon_update(delta_time);
    activePlayer->weapons[choosenWeapon].weapon_draw(&raster);

    if(activePlayer->weapons[choosenWeapon].general_life() == 0){
        clickable = 1;
        if(cng){
            activePlayer = &player2;
            opositePlayer = &player1;
            cng = 1;
        }else {
            activePlayer = &player1;
            opositePlayer = &player2;
            cng = 0;
        }
    }

    player1.y = groundLevel[(int)player1.x / CHUNK] - 10 > RASTER_HEIGHT ? RASTER_HEIGHT - 10: groundLevel[(int)player1.x / CHUNK] - 10;
    player2.y = groundLevel[(int)player2.x / CHUNK] - 10 > RASTER_HEIGHT ? RASTER_HEIGHT - 10 : groundLevel[(int)player2.x / CHUNK] - 10;


    rafgl_raster_draw_spritesheet(&raster, &(player1.sprite), 1, 0, player1.x - 20, player1.y - 10);
    rafgl_raster_draw_spritesheet(&raster, &(player2.sprite), 3, 0, player2.x - 20, player2.y - 10);

    /* shift + s snima raster */
    if(game_data->keys_pressed[RAFGL_KEY_S] && game_data->keys_down[RAFGL_KEY_LEFT_SHIFT])
    {
        sprintf(save_file, "save%d.png", save_file_no++);
        rafgl_raster_save_to_png(&raster, save_file);
    }

    printNum(player1.points, (int)player1.x, 5);
    printNum(player2.points, (int)player2.x, 5);

    /* update-uj teksturu*/
    if(!game_data->keys_down[RAFGL_KEY_SPACE])
        rafgl_texture_load_from_raster(&texture, &raster);
    else
        rafgl_texture_load_from_raster(&texture, &raster2);

    for(int j = 0; j < 10; j++)
    for(int i = 1; i < RASTER_WIDTH / CHUNK - 1; i++){
        if(groundLevel[i] > groundLevel[i + 1] + 1)
        {
            groundLevel[i]--;
            groundLevel[i + 1]++;
        }
        if(groundLevel[i] > groundLevel[i - 1] + 1)
        {
            groundLevel[i]--;
            groundLevel[i - 1]++;
        }
    }
}


void main_state_render(GLFWwindow *window, void *args)
{
    /* prikazi teksturu */
    rafgl_texture_show(&texture);
}


void main_state_cleanup(GLFWwindow *window, void *args)
{
    rafgl_raster_cleanup(&raster);
    rafgl_raster_cleanup(&raster2);
    rafgl_texture_cleanup(&texture);

}

void printNum(int num, int x, int y){
    int dx = 0;
    while(num){
        printDig(num % 10, x - dx, y);
        num /= 10;
        dx += 30;
    }
}

void printDig(int dig, int x, int y){
    rafgl_raster_draw_spritesheet(&raster, &(number), dig, 0, x, y);
}


