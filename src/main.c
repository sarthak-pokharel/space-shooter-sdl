#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_render.h>

struct Bullets{
    SDL_Rect each[5000];
    int length;
};


char *the_game_name = "SPACE SHOOTER";
char pathbuffer[256];
char win_closed = 0;
int del_count=0;
int bullet_delc_interval = 10;
int bullet_vel_perframe = 3;
int mouseX, mouseY;
int WW, WH;
SDL_Rect hero;
struct Bullets game_bullets;
int max_bullets_per_screen = 450;


//Texture ,surfaces, renderers, windows
SDL_Window* win;
SDL_Surface* herosurface;
SDL_Surface* bulletsurface;
SDL_Texture* herotexture;
SDL_Texture* bullettexture;
SDL_Renderer* rend;





void updateHeroPos(SDL_Rect *hero){
    hero->x = mouseX - hero->w/2;
    hero->y = WH-hero->h;
}

void updateBullets(struct Bullets *bullets){
    for(int i =0; i<bullets->length; i++){
        bullets->each[i].y -= bullet_vel_perframe;
    }
}

void generateBullet(struct Bullets *bullets){
    SDL_Rect temp_blt, tmpswp;
    int i;
    temp_blt.w = 15;
    temp_blt.h = 50;
    
    if(bullets->length >=max_bullets_per_screen){
        bullets->length -= 1;
        for(i = 1; i<max_bullets_per_screen;i++){
            tmpswp = bullets->each[i];
            bullets->each[i] = bullets->each[i-1];
            bullets->each[i-1] = tmpswp;
        }
        // return;
    }
    temp_blt.x = hero.x + hero.w*0.35;
    temp_blt.y = hero.y-hero.h/2;
    bullets->each[bullets->length] = temp_blt;
    bullets->length = (bullets->length) + 1; 
    SDL_QueryTexture(bullettexture, NULL, NULL, &temp_blt.w, &temp_blt.h);
    
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

void start_shooting(){
    int i;
    while(!win_closed){
        SDL_GetMouseState(&mouseX,&mouseY);
        SDL_GetWindowSize(win,&WW, &WH);
        
        event_management_loop();

        if(del_count%(bullet_delc_interval) == 0){
            generateBullet(&game_bullets);
        }
        updateHeroPos(&hero);
        updateBullets(&game_bullets);

        SDL_RenderClear(rend);
        SDL_RenderCopy(rend, herotexture, NULL, &hero);
        for(i=0;i<game_bullets.length; i++){
            SDL_RenderCopy(rend, bullettexture, NULL, game_bullets.each+i);
        }
        SDL_RenderPresent(rend);
        SDL_Delay(1000 / 60);
        del_count += 1;
    };
}

void main()
{
    Uint32 render_flags;
    int i;

    getcwd(pathbuffer,256);
    printf("fpath(%s)\n", pathbuffer);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }

    win = SDL_CreateWindow(the_game_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, 0);
    render_flags = SDL_RENDERER_ACCELERATED;
    rend = SDL_CreateRenderer(win, -1, render_flags);
    herosurface = IMG_Load("res/spaceship-png-icon-17267.png");
    bulletsurface = IMG_Load("res/kindpng_2969110.png");
    herotexture = SDL_CreateTextureFromSurface(rend, herosurface);
    bullettexture = SDL_CreateTextureFromSurface(rend, bulletsurface);
    
    SDL_FreeSurface(herosurface);
    SDL_FreeSurface(bulletsurface);

    SDL_QueryTexture(herotexture, NULL, NULL, &hero.w, &hero.h);
    
    hero.w = 50;
    hero.h = 50;
    

    printf("SDL window is on!\n");

    start_shooting();
    
}
