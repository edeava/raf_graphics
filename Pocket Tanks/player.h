#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED
#include <game_constants.h>

#define WEAPONS 5
#define HEALTH 100

typedef struct _player_t{
    int points;
    float x, y, angle, speed, azimut;
    weapon_t weapons[WEAPONS];
}player_t;

void initPlayer(player_t *player, int number){
    if(number == 1){
        player->x = 5 * RASTER_WIDTH / 6;
        player->y = 2 * RASTER_HEIGHT / 3;
        player->azimut = -0.75f;
    }else{
        player->x = 5 * RASTER_WIDTH / 6;
        player->y = 2 * RASTER_HEIGHT / 3;
        player->azimut = -0.25f;
    }
    player->angle = player->azimut *  M_PI *  2.0f;
    player->speed = ( 0.3f + 0.7 * randf()) * 10;
}

#endif
