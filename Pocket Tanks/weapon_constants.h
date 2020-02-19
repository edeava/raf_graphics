#ifndef WEAPON_CONSTANTS_H_INCLUDED
#define WEAPON_CONSTANTS_H_INCLUDED
#include <game_constants.h>
#include <rafgl.h>
#include <main_state.h>
#include <glad/glad.h>
#include <math.h>


#define MAX_BULLETS 10
#define MAX_HE 1
#define MAX_MISSLE 1
#define MAX_TESLA 1

const max_bullets = MAX_BULLETS;


#define CHUNK 2

#define WEAPONS 5
#define HEALTH 100

rafgl_raster_t *rasterT;

typedef struct _weapon_t
{
    float x, y, dx, dy;
    int life;
    int damage;
    rafgl_spritesheet_t sprite;
    void (*weapon_check)(struct _weapon_t*);
    void (*weapon_draw)(rafgl_raster_t*);
    void (*weapon_update)(float);
    void (*weapon_fire)(void *);
    int (*general_life)();
} weapon_t;

typedef struct _player_t{
    int points;
    float x, y, angle, speed, azimut, sCoef;
    rafgl_spritesheet_t sprite;
    weapon_t weapons[WEAPONS];
}player_t;

weapon_t bullets[MAX_BULLETS], hes[MAX_HE], missles[MAX_MISSLE], teslas[MAX_TESLA];
int ground[RASTER_WIDTH / CHUNK], teslaForm = 0;
char button;
float counter = 0.0f;

void makeBullet(weapon_t *bullet);
void bullet_check(weapon_t * bullet);
void bullet_draw(rafgl_raster_t *raster);
void bullet_update(float delta_time);
void bullet_fire(void *);
int bullet_general_life();

void makeMissle(weapon_t *missle);
void missle_check(weapon_t * missle);
void missle_draw(rafgl_raster_t *raster);
void missle_update(float delta_time);
void missle_fire(void *);
int missle_general_life();

void makeHe(weapon_t *he);
void he_check(weapon_t * he);
void he_draw(rafgl_raster_t *raster);
void he_update(float delta_time);
void he_fire(void *);
int he_general_life();

void makeTesla(weapon_t *tesla);
void tesla_check(weapon_t * tesla);
void tesla_draw(rafgl_raster_t *raster);
void tesla_update(float delta_time);
void tesla_fire(void *);
int tesla_general_life();
/*IMPLEMENTATION*/

void makeBullet(weapon_t *bullet){
    bullet->weapon_check = &bullet_check;
    bullet->weapon_draw = &bullet_draw;
    bullet->weapon_update = &bullet_update;
    bullet->weapon_fire = &bullet_fire;
    bullet->general_life = &bullet_general_life;
    bullet->damage = 20;
    for(int i = 0; i < max_bullets; i++)
        bullets[i].damage = 20;
}

void initData(char b){
    button = b;
}

void initG(int gameGround[], int n){
    for(int i = 0; i < n; i++)
        ground[i] = gameGround[i];
}

void initPlayer(player_t *player, int number, rafgl_raster_t *r){
    weapon_t bullet, he, missle, tesla;
    makeMissle(&missle);
    makeHe(&he);
    makeBullet(&bullet);
    makeTesla(&tesla);
    rasterT = r;
    if(number == 1){
        player->x = 5 * RASTER_WIDTH / 6;
        player->y = 2 * RASTER_HEIGHT / 3;
        player->azimut = 0.625f;
    }else{
        player->x = RASTER_WIDTH / 6;
        player->y = 2 * RASTER_HEIGHT / 3;
        player->azimut = 0.845f;
    }
    player->sCoef = 0.5;
    player->weapons[0] = bullet; player->weapons[1] = he; player->weapons[2] = missle; player->weapons[3] = tesla;
    rafgl_spritesheet_init(&(player->sprite), "res/images/tenk.png", 10, 1);
    player->points = 0;
}

void bullet_check(weapon_t * bullet){
    if(bullet->y > ground[(int)(bullet->x / CHUNK)]){
        damage(bullet->damage, (int)(bullet->x / CHUNK));
        bullet->life = 0;
    }
}

void bullet_draw(rafgl_raster_t *raster)
{
    int i;
    weapon_t p;
    for(i = 0; i < MAX_BULLETS; i++)
    {
        p = bullets[i];
        if(p.life <= 0) continue;
        rafgl_raster_draw_line(raster, p.x - p.dx, p.y - p.dy, p.x, p.y, rafgl_RGB(255, 255,  255));
    }
}

void bullet_update(float delta_time)
{
    int i;
    for(i = 0; i < MAX_BULLETS; i++)
    {
        if(bullets[i].life <= 0) continue;

        //bullets[i].life--;

        bullets[i].x += bullets[i].dx;
        bullets[i].y += bullets[i].dy;
        bullets[i].dx *= 0.995f;
        bullets[i].dy *= 0.995f;
        bullets[i].dy += 0.1;

        if(bullets[i].x < 0)
        {
            bullets[i].x = 0;
            bullets[i].life = 0;
            //particles[i].dx = (rafgl_abs_m(particles[i].dx)) * randf() * elasticity;
        }

        /*if(particles[i].y < 0)
        {
            particles[i].y = 0;
            particles[i].dy = (rafgl_abs_m(particles[i].dy)) * randf() * elasticity;
        }*/

        if(bullets[i].x >= RASTER_WIDTH)
        {
            bullets[i].x = RASTER_WIDTH - 1;
            bullets[i].life = 0;
           // particles[i].dx = (rafgl_abs_m(particles[i].dx)) * randf() * (-elasticity);
        }

        if(bullets[i].y >= RASTER_HEIGHT)
        {
            bullets[i].y = RASTER_HEIGHT - 1;
            bullets[i].life = 0;
            //particles[i].dy = (rafgl_abs_m(particles[i].dy)) * randf() * (-elasticity);
        }

        bullet_check(&bullets[i]);

    }
}

void bullet_fire(void *p){
    player_t *player = (player_t*)p;
    for(int i = 0; (i < max_bullets) /*&& gen*/; i++)
        {
            if(bullets[i].life <= 0)
            {
                float coef = 1;
                bullets[i].life = 1;
                bullets[i].x = player->x;
                bullets[i].y = player->y;

                player->angle = player->azimut *  M_PI *  2.0f;
                player->speed = player->sCoef * ( 0.3f + 15 + randf() * 4);
                bullets[i].dx = cosf(player->angle) * player->speed;
                bullets[i].dy = sinf(player->angle) * player->speed;
                //gen--;

            }
        }
}

int bullet_general_life(){
    int ret = 0;
    for(int i = 0; i < max_bullets; i++)
        ret = ret || bullets[i].life;
    return ret;
}

void makeHe(weapon_t *he){
    he->weapon_check = &he_check;
    he->weapon_draw = &he_draw;
    he->weapon_update = &he_update;
    he->weapon_fire = &he_fire;
    he->general_life = &he_general_life;
    he->damage = 70;
    for(int i = 0; i < MAX_HE; i++){
        hes[i].damage = 70;
        rafgl_spritesheet_init(&(hes[i].sprite), "res/images/he.png", 1, 1);
    }
}
void he_check(weapon_t * he){
    if(he->y > ground[(int)(he->x / CHUNK)]){
        damage(he->damage, (int)(he->x / CHUNK));
        damage(he->damage, (int)((he->x + 100)/ CHUNK));
        damage(he->damage, (int)((he->x - 100)/ CHUNK));
        he->life = 0;
    }
}
void he_draw(rafgl_raster_t *raster){
    int i;
    weapon_t p;
    for(i = 0; i < MAX_HE; i++)
    {
        p = hes[i];
        if(p.life <= 0) continue;
        rafgl_raster_draw_spritesheet(rasterT, &(hes[i].sprite), 0, 0, hes[i].x, hes[i].y);
    }

}
void he_update(float delta_time){
    int i;
    for(i = 0; i < MAX_HE; i++)
    {
        if(hes[i].life <= 0) continue;

        hes[i].x += hes[i].dx;
        hes[i].y += hes[i].dy;
        hes[i].dx *= 0.995f;
        hes[i].dy *= 0.995f;
        hes[i].dy += 0.1;

        if(hes[i].x < 0)
        {
            hes[i].x = 0;
            hes[i].life = 0;
        }

        if(hes[i].x >= RASTER_WIDTH)
        {
            hes[i].x = RASTER_WIDTH - 1;
            hes[i].life = 0;
        }

        if(hes[i].y >= RASTER_HEIGHT)
        {
            hes[i].y = RASTER_HEIGHT - 1;
            hes[i].life = 0;
        }

        he_check(&hes[i]);

    }
}
void he_fire(void *p){
    player_t *player = (player_t*)p;
    for(int i = 0; (i < MAX_HE); i++)
        {
            if(hes[i].life <= 0)
            {
                float coef = 1;
                hes[i].life = 1;
                hes[i].x = player->x;
                hes[i].y = player->y;

                player->angle = player->azimut *  M_PI *  2.0f;
                player->speed = player->sCoef * ( 0.3f + 15);
                hes[i].dx = cosf(player->angle) * player->speed;
                hes[i].dy = sinf(player->angle) * player->speed;
            }
        }
}
int he_general_life(){
    int ret = 0;
    for(int i = 0; i < MAX_HE; i++)
        ret = ret || hes[i].life;
    return ret;
}

void makeMissle(weapon_t *missle){
    missle->weapon_check = &missle_check;
    missle->weapon_draw = &missle_draw;
    missle->weapon_update = &missle_update;
    missle->weapon_fire = &missle_fire;
    missle->general_life = &missle_general_life;
    missle->damage = 50;
    for(int i = 0; i < MAX_MISSLE; i++){
        missles[i].damage = missle->damage;
        rafgl_spritesheet_init(&(missles[i].sprite), "res/images/missle.png", 10, 1);
    }
}
void missle_check(weapon_t * missle){
    if(missle->y > ground[(int)(missle->x / CHUNK)]){
        damage(missle->damage, (int)(missle->x / CHUNK));
        damage(missle->damage, (int)((missle->x + 20)/ CHUNK));
        damage(missle->damage, (int)((missle->x - 20)/ CHUNK));
        missle->life = 0;
    }
}
void missle_draw(rafgl_raster_t *raster){
    int i, angle = 0;
    weapon_t p;
    for(i = 0; i < MAX_MISSLE; i++)
    {
        p = missles[i];
        if(p.life <= 0) continue;
        float t = p.dy / p.dx;
        float arcus = atan(t);

        if(abs(p.dx) > (abs(p.dy) * 10.15317f) && (p.dx > 0))
            angle = 5;
        else if(abs(p.dx) > (abs(p.dy) * 10.15317f) && (p.dx < 0))
            angle = 7;
        else if(abs(p.dy) > (abs(p.dx) * 10.15317f) && (p.dy > 0))
            angle = 6;
        else if(abs(p.dx) > (abs(p.dy) * 10.15317f) && (p.dy < 0))
            angle = 4;
        else{
            if(p.dx > 0 && p.dy > 0)
                angle = 2;
            else if(p.dx < 0 && p.dy < 0)
                angle = 0;
            else if(p.dx > 0 && p.dy < 0)
                angle = 1;
            else if(p.dx < 0 && p.dy > 0)
                angle = 3;
        }
        /*if((arcus > (12 * M_PI / 16)) && (arcus < (13 * M_PI / 16)) && p.dy < 0)
            angle = 4;
        else if((arcus > (13 * M_PI / 16)) && (arcus < (15 * M_PI / 16)) && p.dy < 0)
            angle = 1;
        else if((arcus < (1 * M_PI / 16)) && (arcus > (15 * M_PI / 16)) && p.dx > 0)
            angle = 5;
        else if((arcus > (1 * M_PI / 16)) && (arcus < (3 * M_PI / 16)) && p.dy > 0)
            angle = 2;
        else if((arcus > (3 * M_PI / 16)) && (arcus < (4 * M_PI / 16)) && p.dy > 0)
            angle = 6;
        else if((arcus > (1 * M_PI / 16)) && (arcus < (3 * M_PI / 16)) && p.dy < 0)
            angle = 0;
        else if((arcus > (13 * M_PI / 16)) && (arcus < (15 * M_PI / 16)) && p.dy > 0)
            angle = 3;
        else if((arcus < (1 * M_PI / 16)) && (arcus > (15 * M_PI / 16)) && p.dx < 0)
            angle = 7;*/
        rafgl_raster_draw_spritesheet(rasterT, &(missles[i].sprite), angle, 0, missles[i].x, missles[i].y);
    }

}
void missle_update(float delta_time){
    int i = 0;
    for(i = 0; i < MAX_MISSLE; i++)
    {
        if(missles[i].life <= 0) continue;
        float dxP, dyP;
        float coef = 50;
        if(button == 0){
            dxP = missles[i].dx;
            dyP = missles[i].dy;
        }

        if(missles[i].dy > 0 && missles[i].dx < 0){
            if(button == 'A'){
                dxP = missles[i].dx + (abs(missles[i].dy)/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
                dyP = missles[i].dy + (abs(missles[i].dx)/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
            }else  if(button == 'D'){
                dxP = missles[i].dx - (abs(missles[i].dy)/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
                dyP = missles[i].dy - (abs(missles[i].dx)/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
            }
        }else if(missles[i].dy < 0 && missles[i].dx < 0){
            if(button == 'A'){
                dxP = missles[i].dx - (abs(missles[i].dy)/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
                dyP = missles[i].dy + (abs(missles[i].dx)/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
            }else  if(button == 'D'){
                dxP = missles[i].dx + (abs(missles[i].dy)/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
                dyP = missles[i].dy - (abs(missles[i].dx)/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
            }
        }else if(missles[i].dy < 0 && missles[i].dx > 0){
            if(button == 'A'){
                dxP = missles[i].dx - (abs(missles[i].dy)/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
                dyP = missles[i].dy - (abs(missles[i].dx)/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
            }else  if(button == 'D'){
                dxP = missles[i].dx + (abs(missles[i].dy)/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
                dyP = missles[i].dy + (abs(missles[i].dx)/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
            }
        }else{
            if(button == 'A'){
                dxP = missles[i].dx + (abs(missles[i].dy)/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
                dyP = missles[i].dy - (abs(missles[i].dx)/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
            }else  if(button == 'D'){
                dxP = missles[i].dx - (abs(missles[i].dy)/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
                dyP = missles[i].dy + (abs(missles[i].dx)/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
            }
        }

        /*if(missles[i].dx * missles[i].dy < 0){
            if(button == 'A'){
                dxP = missles[i].dx + (missles[i].dy/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
                dyP = missles[i].dy - (missles[i].dx/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
            }else  if(button == 'D'){
                dxP = missles[i].dx - (missles[i].dy/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
                dyP = missles[i].dy + (missles[i].dx/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
            }

        }else if(missles[i].dx * missles[i].dy > 0){
            if(button == 'A'){
                dxP = missles[i].dx - (missles[i].dy/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
                dyP = missles[i].dy + (missles[i].dx/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
            }else  if(button == 'D'){
                dxP = missles[i].dx + (missles[i].dy/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
                dyP = missles[i].dy - (missles[i].dx/(sqrt(missles[i].dx*missles[i].dx + missles[i].dy*missles[i].dy)) * delta_time * coef);
            }
        }*/



        float k = sqrt((dxP * dxP + dyP * dyP) / (missles[i].dx * missles[i].dx + missles[i].dy * missles[i].dy));

        missles[i].dx = dxP / k;
        missles[i].dy = dyP / k;

        missles[i].x += missles[i].dx;
        missles[i].y += missles[i].dy;
        /*missles[i].dx *= 0.995f;
        missles[i].dy *= 0.995f;
        missles[i].dy += 0.1;*/

        if(missles[i].x < 0)
        {
            missles[i].x = 0;
            missles[i].life = 0;
        }

        if(missles[i].x >= RASTER_WIDTH)
        {
            missles[i].x = RASTER_WIDTH - 1;
            missles[i].life = 0;
        }

        if(missles[i].y >= RASTER_HEIGHT)
        {
            missles[i].y = RASTER_HEIGHT - 1;
            missles[i].life = 0;
        }

        missle_check(&missles[i]);
        //missle_draw(&rasterT);
    }
}
void missle_fire(void *p){
    player_t *player = (player_t*)p;
    for(int i = 0; (i < MAX_MISSLE); i++)
        {
            if(missles[i].life <= 0)
            {
                //float coef = 10;
                missles[i].life = 1;
                missles[i].x = player->x;
                missles[i].y = player->y;

                player->angle = player->azimut *  M_PI *  2.0f;
                player->speed = player->sCoef * (0.3f + 12);
                missles[i].dx = cosf(player->angle) * player->speed;
                missles[i].dy = sinf(player->angle) * player->speed;
            }
        }
}
int missle_general_life(){
    int ret = 0;
    for(int i = 0; i < MAX_MISSLE; i++)
        ret = ret || missles[i].life;
    return ret;
}

void makeTesla(weapon_t *tesla){
    tesla->weapon_check = &tesla_check;
    tesla->weapon_draw = &tesla_draw;
    tesla->weapon_update = &tesla_update;
    tesla->weapon_fire = &tesla_fire;
    tesla->general_life = &tesla_general_life;
    tesla->damage = 30;
    for(int i = 0; i < MAX_TESLA; i++){
        teslas[i].damage = tesla->damage;
        rafgl_spritesheet_init(&(teslas[i].sprite), "res/images/tesla.png", 4, 1);
    }
}
void tesla_check(weapon_t * tesla){
    if(tesla->y > ground[(int)(tesla->x / CHUNK)]){
        damage(tesla->damage, (int)((tesla->x + 20)/ CHUNK));
        damage(tesla->damage, (int)((tesla->x - 20)/ CHUNK));
        tesla->life = 0;
    }
}
void tesla_draw(rafgl_raster_t *raster){
    int i;
    weapon_t p;
    for(i = 0; i < MAX_TESLA; i++)
    {
        p = teslas[i];
        if(p.life <= 0) continue;
        rafgl_raster_draw_spritesheet(rasterT, &(teslas[i].sprite), teslaForm, 0, teslas[i].x, teslas[i].y);
    }

}
void tesla_update(float delta_time){
    int i;
    for(i = 0; i < MAX_TESLA; i++)
    {
        if(teslas[i].life <= 0) continue;

        teslas[i].x += teslas[i].dx;
        teslas[i].y += teslas[i].dy;
        teslas[i].dx *= 0.995f;
        teslas[i].dy *= 0.995f;
        teslas[i].dy += 0.1;

        if(teslas[i].x < 0)
        {
            teslas[i].x = 0;
            teslas[i].life = 0;
        }

        if(teslas[i].x >= RASTER_WIDTH)
        {
            teslas[i].x = RASTER_WIDTH - 1;
            teslas[i].life = 0;
        }

        if(teslas[i].y >= RASTER_HEIGHT)
        {
            teslas[i].y = RASTER_HEIGHT - 1;
            teslas[i].life = 0;
        }

        tesla_check(&teslas[i]);
        if(counter >= 0.6){
            teslaForm = (teslaForm + 1) % 4;
        }
        counter += delta_time;
    }
}
void tesla_fire(void *p){
    player_t *player = (player_t*)p;
    for(int i = 0; (i < MAX_TESLA); i++)
        {
            if(teslas[i].life <= 0)
            {
                float coef = 1;
                teslas[i].life = 1;
                teslas[i].x = player->x;
                teslas[i].y = player->y;

                player->angle = player->azimut *  M_PI *  2.0f;
                player->speed = player->sCoef * ( 0.3f + 15);
                teslas[i].dx = cosf(player->angle) * player->speed;
                teslas[i].dy = sinf(player->angle) * player->speed;
            }
        }
}
int tesla_general_life(){
    int ret = 0;
    for(int i = 0; i < MAX_MISSLE; i++)
        ret = ret || teslas[i].life;
    return ret;
}

#endif // WEAPON_CONSTANTS_H_INCLUDED
