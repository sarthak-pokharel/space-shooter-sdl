#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_render.h>

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
    struct Enemy each[50];
    int length;
};


char *the_game_name = "SPACE SHOOTER";
char pathbuffer[256];
char win_closed = 0;
int del_count=0;
int bullet_delc_interval = 10,
    enemy_delc_interval = 200;
int bullet_vel_perframe = 3,
    enemies_vel_perframe = 1;
int mouseX, mouseY;
int WW, WH;
SDL_Rect hero;
struct Bullets game_bullets;
struct Enemies game_enemies;
int max_bullets_per_screen = 13,
    max_enemies_per_screen = 5;


//Texture ,surfaces, renderers, windows
SDL_Window* win;
SDL_Surface* herosurface;
SDL_Surface* bulletsurface;
SDL_Texture* enemysurface;

SDL_Texture* herotexture;
SDL_Texture* bullettexture;
SDL_Texture* enemytexture;
SDL_Renderer* rend;


void collision_control(){
    
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
    if(enemies->length >= max_enemies_per_screen) return;

    int i;
    temp_enemy.context.w = 30;
    temp_enemy.context.h = 50;
    temp_enemy.dead = 0;


    temp_enemy.context.x = rand()%WW;
    temp_enemy.context.y = -10;
    enemies->each[enemies->length] = temp_enemy;
    enemies->length = enemies->length + 1;
    SDL_QueryTexture(enemytexture, NULL, NULL, &temp_enemy.context.w, &temp_enemy.context.h);
}

void generateBullet(struct Bullets *bullets){
    struct Bullet temp_blt, tmpswp;
    int i;
    temp_blt.context.w = 15;
    temp_blt.context.h = 50;
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
    updateHeroPos(&hero);
    updateBullets(&game_bullets);
    updateEnemies(&game_enemies);

    collision_control();

    SDL_RenderClear(rend);
    SDL_RenderCopy(rend, herotexture, NULL, &hero);
    for(i=0;i<game_bullets.length; i++){
        SDL_RenderCopy(rend, bullettexture, NULL, &game_bullets.each[i].context);
    }
    for(i=0;i<game_enemies.length; i++){
        SDL_RenderCopy(rend, enemytexture, NULL, &game_enemies.each[i].context);
    }
    SDL_RenderPresent(rend);
    SDL_Delay(1000 / 60);
    del_count += 1;
}

void start_animation(){
    while(!win_closed){
        SDL_GetMouseState(&mouseX,&mouseY);
        SDL_GetWindowSize(win,&WW, &WH);
        
        event_management_loop();

        game_loop();
    };
}

char* hero_img_path = "res/spaceship-png-icon-17267.png";
char* bullet_img_path = "res/kindpng_2969110.png";

char* enemy_img_path = "res/PngItem_852440.png";

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
