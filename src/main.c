#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>
#define defendelcinterval 50

struct Bullet {
    SDL_Rect context;
    int broken;
};
struct Bullets{
    struct Bullet each[50];
    int length;
};
struct Enemy {
    SDL_Rect context;
    int dead;
};
struct Enemies {
    struct Enemy each[300];
    int length;
};

int max_level_enemy_max = 250;
char *the_game_name = "SPACE SHOOTER";
char pathbuffer[256];
char win_closed = 0;
int del_count=0;

int def_font_size = 16;

int bullet_delc_interval = 10,
    enemy_delc_interval = defendelcinterval,
    levelup_interval = 50;
int min_enemy_delc = 15;
int bullet_vel_perframe = 5,
    enemies_vel_perframe = 3;
int mouseX, mouseY;
int WW, WH;
SDL_Rect hero;
struct Bullets game_bullets;
struct Enemies game_enemies;
int max_bullets_per_screen = 13,
    max_enemies_per_screen = 5;

int maxEnemyHealth = 3;

int game_over = 0;

char* font_path = "res/TURNB___.TTF";
char* hero_img_path = "res/spaceship-png-icon-17267.png";
char* bullet_img_path = "res/kindpng_2969110.png";
char* enemy_img_path = "res/PngItem_852440.png";



//Texture ,surfaces, renderers, windows
SDL_Window* win;
SDL_Surface* herosurface;
SDL_Surface* bulletsurface;
SDL_Texture* enemysurface;

SDL_Texture* herotexture;
SDL_Texture* bullettexture;
SDL_Texture* enemytexture;
SDL_Renderer* rend;
TTF_Font* fontname;
int gameScore = 0, highScore = 0;

void show_text(char *txt, int x, int y){
    SDL_Color White = {255, 255, 255};
    SDL_Surface* surfaceMessage;
    SDL_Texture* Message;
    SDL_Rect Message_rect;

    surfaceMessage = TTF_RenderText_Solid(fontname, txt, White); 
    Message = SDL_CreateTextureFromSurface(rend, surfaceMessage);
    Message_rect.x = x;
    Message_rect.y = y; 
    Message_rect.w = surfaceMessage->w;
    Message_rect.h = surfaceMessage->h;
    
    SDL_RenderCopy(rend, Message, NULL, &Message_rect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

void declare_gameover(){
    game_over = 1;
    printf("Game is over\n ");
}
void collision_control(){
    int i, j;
    SDL_Rect bulpos;
    SDL_Rect enempos;
    for(i=0;i<game_enemies.length; i++){
        if(game_enemies.each[i].dead == maxEnemyHealth) continue;
        enempos = game_enemies.each[i].context;
        if(enempos.y >=WH){
            declare_gameover();
            continue;
        }
        for(j=0; j<game_bullets.length; j++){
            if(game_bullets.each[j].broken) continue;
            bulpos = game_bullets.each[j].context;
            if((bulpos.x >= (enempos.x-enempos.w)) && (bulpos.x <= (enempos.x+enempos.w))) {
                if(bulpos.y <= (enempos.y+enempos.h/2)){
                    // printf("collision detected, enemy crash b(%d %d) e(%d %d)\n",bulpos.x,bulpos.y, enempos.x, enempos.y);
                    game_enemies.each[i].dead += 1;
                    if(game_enemies.each[i].dead == maxEnemyHealth){
                        gameScore += 10;
                    }
                    game_bullets.each[j].broken = 1;
                }
            }
        }
    }
}

void updateHeroPos(SDL_Rect *hero){
    hero->x = mouseX - hero->w/2;
    hero->y = WH-hero->h;
}
void updateEnemies(struct Enemies *enemies){
    int i;
    for(i=0; i<enemies->length; i++){
        enemies->each[i].context.y += enemies_vel_perframe;
    }
}

void updateBullets(struct Bullets *bullets){
    int i;
    for(i =0; i<bullets->length; i++){
        bullets->each[i].context.y -= bullet_vel_perframe;
    }
}

void generateEnemy(struct Enemies *enemies){
    struct Enemy temp_enemy, tmpswp;

    int i;
    temp_enemy.context.w = 30;
    temp_enemy.context.h = 50;
    temp_enemy.dead = 0;

    if(enemies->length >=max_enemies_per_screen){
        enemies->length -= 1;
        for(i = 1; i<max_enemies_per_screen;i++){
            tmpswp = enemies->each[i];
            enemies->each[i] = enemies->each[i-1];
            enemies->each[i-1] = tmpswp;
        }
        // return;
    }

    temp_enemy.context.x = rand()%(WW-temp_enemy.context.w);
    temp_enemy.context.y = -10;
    enemies->each[enemies->length] = temp_enemy;
    enemies->length = enemies->length + 1;
    SDL_QueryTexture(enemytexture, NULL, NULL, &temp_enemy.context.w, &temp_enemy.context.h);
}

void generateBullet(struct Bullets *bullets){
    struct Bullet temp_blt, tmpswp;
    int i;
    temp_blt.context.w = 15/2;
    temp_blt.context.h = 50/2;
    temp_blt.broken = 0;
    
    if(bullets->length >=max_bullets_per_screen){
        bullets->length -= 1;
        for(i = 1; i<max_bullets_per_screen;i++){
            tmpswp = bullets->each[i];
            bullets->each[i] = bullets->each[i-1];
            bullets->each[i-1] = tmpswp;
        }
        // return;
    }
    temp_blt.context.x = hero.x + hero.w*0.35;
    temp_blt.context.y = hero.y-hero.h/2;
    bullets->each[bullets->length] = temp_blt;
    bullets->length = (bullets->length) + 1; 
    SDL_QueryTexture(bullettexture, NULL, NULL, &temp_blt.context.w, &temp_blt.context.h);
}

void event_management_loop(){
    SDL_Event win_event;
    while(SDL_PollEvent(&win_event)){
        if(
            (win_event.type == SDL_KEYDOWN && win_event.key.keysym.sym == SDLK_ESCAPE) 
            ||
            (win_event.type == SDL_WINDOWEVENT && win_event.window.event == SDL_WINDOWEVENT_CLOSE)
            ){
                win_closed = 1;
                printf("Quit sdl\n");
                SDL_DestroyWindow(win);
                SDL_Quit();
        };
        if(
            (win_event.type == SDL_KEYDOWN) && (win_event.key.keysym.sym == SDLK_r)
        ){
            game_over = 0;
        }
    }
}

void game_loop(){
    int i;
    if(del_count%(bullet_delc_interval) == 0){
        generateBullet(&game_bullets);
    }
    if(del_count%(enemy_delc_interval) == 0){
        generateEnemy(&game_enemies);
    }
    if(del_count%(levelup_interval) == 0){
        if(enemy_delc_interval<=min_enemy_delc){
            enemy_delc_interval -= 1;
        }
    }
    updateHeroPos(&hero);
    updateBullets(&game_bullets);
    updateEnemies(&game_enemies);
    collision_control();


    SDL_RenderClear(rend);
    char txtToShow[100];
    float del_y = 20;
    sprintf(txtToShow, "Score: %d", gameScore);
    show_text(txtToShow, 10, 20);
    sprintf(txtToShow, "High Score: %d", highScore);
    show_text(txtToShow, 10, 20+del_y);
    SDL_RenderCopy(rend, herotexture, NULL, &hero);
    for(i=0;i<game_bullets.length; i++){
        if(game_bullets.each[i].broken) continue;
        SDL_RenderCopy(rend, bullettexture, NULL, &game_bullets.each[i].context);
    }
    for(i=0;i<game_enemies.length; i++){
        if(game_enemies.each[i].dead == maxEnemyHealth) continue;
        SDL_RenderCopy(rend, enemytexture, NULL, &game_enemies.each[i].context);
    }
    del_count += 1;
}

void start_animation(){
    while(!win_closed){
        SDL_GetMouseState(&mouseX,&mouseY);
        SDL_GetWindowSize(win,&WW, &WH);
        event_management_loop();

        if(game_over){
            show_text("Game Over, press R to restart",WW/2-150, WH/2);
            memset(&game_enemies,0, sizeof(game_enemies));
            memset(&game_bullets,0, sizeof(game_bullets));
            enemy_delc_interval = defendelcinterval;
            if(highScore < gameScore){
                highScore = gameScore;
            }
            gameScore = 0;
        }else{
            game_loop();
        }
        SDL_RenderPresent(rend);
        SDL_Delay(1000 / 60);
    };
}

void main()
{
    Uint32 render_flags;
    int i;
    time_t t1;


    srand ( (unsigned) time (&t1));
    getcwd(pathbuffer,256);
    printf("fpath(%s)\n", pathbuffer);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }

    win = SDL_CreateWindow(the_game_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, 0);
    SDL_ShowCursor(0);

    if(TTF_Init()<0){
        printf("error initializing TTF %s\n", TTF_GetError());
    }
    SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN);

    fontname = TTF_OpenFont(font_path, def_font_size);
    if(fontname == NULL){
        printf("Error loading font :%s\n", TTF_GetError());
    }

    render_flags = SDL_RENDERER_ACCELERATED;
    rend = SDL_CreateRenderer(win, -1, render_flags);
    herosurface = IMG_Load(hero_img_path);
    bulletsurface = IMG_Load(bullet_img_path);
    enemysurface = IMG_Load(enemy_img_path);
    herotexture = SDL_CreateTextureFromSurface(rend, herosurface);
    bullettexture = SDL_CreateTextureFromSurface(rend, bulletsurface);
    enemytexture = SDL_CreateTextureFromSurface(rend, enemysurface);
    
    SDL_FreeSurface(herosurface);
    SDL_FreeSurface(bulletsurface);
    SDL_FreeSurface(enemysurface);

    SDL_QueryTexture(herotexture, NULL, NULL, &hero.w, &hero.h);
    hero.w = 50;
    hero.h = 50;

    printf("The game is on!\n");

    start_animation();
}
