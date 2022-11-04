// SDL_Project.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <string>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_TTF.h"
#include "SDL_mixer.h"
#include "InputModule.h"
#include "GSLogoState.h"

using namespace std;

///////// Definicion de estructuras /////////////
#include "StructsDef.h"
///////// Definicion de estructuras /////////////

///////// Variables y Constantes Globales /////////////
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_bool isGameRunning = SDL_TRUE;
float time_multiplier = 1.0f;
InputState gameInputState;
SpriteAssets spritesAssets;
TextAssets textAssets;
BgmAssets musicAssets;
GameStages gameStages;
float VelX = 0.3;
float VelY = 0.3;
float acceleration = 1;
int puntoRojo = 0;
int puntoAzul = 0;
bool restartCounter = false;

ResourceManager resourceManager;

// Todos los ID de cada sprite
int indexSpriteID = 0;
int indexSpritePelotaID = 1;
int indexPaddleBlueID = 2;
int indexPaddleRedID = 3;
int blueTextID = 4;
int redTextID = 5;
int orangeTextID = 6;

///////// Variables y Constantes Globales /////////////

///////// Funciones de inicializacion y destruccion /////////////
void initEngine()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);

    TTF_Init();

    int initted = Mix_Init(MIX_INIT_MP3);
    if ((initted & MIX_INIT_MP3) != MIX_INIT_MP3)
    {
        cout << "Mix_Init: Failed to init required ogg and mod support!" << endl;
        cout << "Mix_Init: " << Mix_GetError() << endl;
        // handle error
    }

    window = SDL_CreateWindow("Pong test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Initializing Resource Manager
    resourceManager.spritesAssets = &spritesAssets;
    resourceManager.textAssets = &textAssets;
    resourceManager.musicAssets = &musicAssets;
    resourceManager.gameStages = &gameStages;
    resourceManager.inputState = &gameInputState;
    resourceManager.renderer = renderer;

    // Starting Game stage
    GameStage logoGameStage;
    logoGameStage.game_stageID = GS_LOGO;
    logoGameStage.stage_name = "Logo";

    gameStages.push(logoGameStage);
}

void destroyEngine()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}

///////// Funciones de inicializacion y destruccion /////////////

///////// Funciones de carga y liberacion de recursos /////////////

void loadAssets()
{

    // Cargo el Logo principal.
    string filePath = "assets/img/logo.png";
    SDL_Texture *texture = IMG_LoadTexture(renderer, filePath.c_str());
    SDL_assert(texture != NULL);
    SDL_Rect dest;
    dest.x = WIDTH >> 2;
    dest.y = 0;
    dest.w = WIDTH >> 1;
    dest.h = HEIGHT >> 1;

    Sprite logoSprite;
    logoSprite.dest = dest;
    logoSprite.texture = texture;
    spritesAssets.push_back(logoSprite);

    indexSpriteID = spritesAssets.size() - 1;

    // cargo la pelota
    string filePathBall = "assets/img/ball.png";
    SDL_Texture *texture2 = IMG_LoadTexture(renderer, filePathBall.c_str());
    SDL_assert(texture2 != NULL);
    SDL_Rect dest2;
    dest2.w = WIDTH * 0.05;                  // controls the width of the rect
    dest2.h = HEIGHT * 0.05;                 // controls the height of the rect
    dest2.x = (WIDTH >> 1) - (dest2.w >> 1); // controls the rect's x coordinate
    dest2.y = HEIGHT >> 1;                   // controls the rect's y coordinte

    Sprite ballSprite;
    ballSprite.dest = dest2;
    ballSprite.texture = texture2;
    spritesAssets.push_back(ballSprite);

    indexSpritePelotaID = spritesAssets.size() - 1;

    // cargo la paleta izquierda (blue)
    string filePathBlue = "assets/img/bluepaddle.png";
    SDL_Texture *texture3 = IMG_LoadTexture(renderer, filePathBlue.c_str());
    SDL_assert(texture3 != NULL);
    SDL_Rect dest3;
    dest3.w = WIDTH * 0.05;                   // controls the width of the rect
    dest3.h = HEIGHT * 0.33;                  // controls the height of the rect
    dest3.x = (WIDTH * 0.1) - (dest3.w >> 1); // controls the rect's x coordinate
    dest3.y = HEIGHT * 0.5;                   // controls the rect's y coordinte

    Sprite blueSprite;
    blueSprite.dest = dest3;
    blueSprite.texture = texture3;
    spritesAssets.push_back(blueSprite);

    indexPaddleBlueID = spritesAssets.size() - 1;

    // cargo la paleta derecha (red)
    string filePathRed = "assets/img/redpaddle.png";
    SDL_Texture *texture4 = IMG_LoadTexture(renderer, filePathRed.c_str());
    SDL_assert(texture4 != NULL);
    SDL_Rect dest4;
    dest4.w = WIDTH * 0.05;                   // controls the width of the rect
    dest4.h = HEIGHT * 0.33;                  // controls the height of the rect
    dest4.x = (WIDTH * 0.9) - (dest4.w >> 1); // controls the rect's x coordinate
    dest4.y = HEIGHT * 0.5;                   // controls the rect's y coordinte

    Sprite redSprite;
    redSprite.dest = dest4;
    redSprite.texture = texture4;
    spritesAssets.push_back(redSprite);

    indexPaddleRedID = spritesAssets.size() - 1;

    // Cargo el texto...
    string fontfilePath = "assets/fonts/Blippo bold.ttf";

    TTF_Font *Sans = TTF_OpenFont(fontfilePath.c_str(), 24); // this opens a font style and sets a size

    SDL_Color Orange = {255, 165, 0}; // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
    SDL_Color Blue = {0, 0, 255};
    SDL_Color Red = {255, 0, 0};

    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(Sans, "", Orange); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
    SDL_Surface *blueScoreMessage = TTF_RenderText_Solid(Sans, "0", Blue);
    SDL_Surface *redScoreMessage = TTF_RenderText_Solid(Sans, "0", Red);

    SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); // now you can convert it into a texture
    SDL_Texture *blueScore = SDL_CreateTextureFromSurface(renderer, blueScoreMessage);
    SDL_Texture *redScore = SDL_CreateTextureFromSurface(renderer, redScoreMessage);

    SDL_Rect Message_rect;                                 // create a rect
    Message_rect.w = WIDTH * 0.65;                         // controls the width of the rect
    Message_rect.h = HEIGHT * 0.15;                        // controls the height of the rect
    Message_rect.x = (WIDTH >> 1) - (Message_rect.w >> 1); // controls the rect's x coordinate
    Message_rect.y = HEIGHT >> 1;                          // controls the rect's y coordinte

    SDL_Rect BlueScore_rect;                                    // create a rect
    BlueScore_rect.w = WIDTH * 0.1;                             // controls the width of the rect
    BlueScore_rect.h = HEIGHT * 0.1;                            // controls the height of the rect
    BlueScore_rect.x = (WIDTH * 0.3) - (BlueScore_rect.w >> 1); // controls the rect's x coordinate
    BlueScore_rect.y = HEIGHT * 0.05;                           // controls the rect's y coordinte

    SDL_Rect RedScore_rect;                                   // create a rect
    RedScore_rect.w = WIDTH * 0.1;                            // controls the width of the rect
    RedScore_rect.h = HEIGHT * 0.1;                           // controls the height of the rect
    RedScore_rect.x = (WIDTH * 0.7) - (RedScore_rect.w >> 1); // controls the rect's x coordinate
    RedScore_rect.y = HEIGHT * 0.05;                          // controls the rect's y coordinte

    Text mainText;
    mainText.font = Sans;
    mainText.color = Orange;
    mainText.surface = surfaceMessage;
    mainText.texture = Message;
    mainText.dest = Message_rect;

    Text blueScoreText;
    blueScoreText.font = Sans;
    blueScoreText.color = Blue;
    blueScoreText.surface = blueScoreMessage;
    blueScoreText.texture = blueScore;
    blueScoreText.dest = BlueScore_rect;

    Text redScoreText;
    redScoreText.font = Sans;
    redScoreText.color = Red;
    redScoreText.surface = redScoreMessage;
    redScoreText.texture = redScore;
    redScoreText.dest = RedScore_rect;

    textAssets.push_back(mainText);
    orangeTextID = textAssets.size() - 1;
    textAssets.push_back(blueScoreText);
    blueTextID = textAssets.size() - 1;
    textAssets.push_back(redScoreText);
    redTextID = textAssets.size() - 1;

    // Cargo Sonidos y BGM
    string soundFilePath = "assets/bgm/thememidi.mp3";
    Mix_Music *music;
    music = Mix_LoadMUS(soundFilePath.c_str());

    Bgm bgm01;
    bgm01.music = music;

    musicAssets.push_back(bgm01);
}

void unloadAssets()
{
    for (int i = 0; i < spritesAssets.size(); i++)
    {
        SDL_DestroyTexture(spritesAssets[i].texture);
    }

    for (int i = 0; i < textAssets.size(); i++)
    {
        SDL_FreeSurface(textAssets[i].surface);
        SDL_DestroyTexture(textAssets[i].texture);
    }

    for (int i = 0; i < musicAssets.size(); i++)
    {
        Mix_FreeMusic(musicAssets[i].music);
    }
}

///////// Funciones de carga y liberacion de recursos /////////////

///////// Funciones de actualizacion y pintado /////////////

void inputUpdate()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {

        switch (event.type)
        {
        case SDL_KEYDOWN:
            onKeyDown(event.key.keysym.sym, gameInputState);
            break;
        case SDL_KEYUP:
            onKeyUp(event.key.keysym.sym, gameInputState);
            break;
        case SDL_MOUSEBUTTONDOWN:
            break;
        case SDL_MOUSEBUTTONUP:
            break;
        case SDL_QUIT:
            isGameRunning = SDL_FALSE;
            break;
        default:
            break;
        }
    }
}

// Para ser usado en distintos contadores..
float timer = 1.0f * 1000; // 1000 ms

void updateGame(float deltaTime)
{
    const float BLINK_SPEED = 5.0f;

    timer -= BLINK_SPEED * deltaTime;

    // para efecto de parpadeo...
    if (timer <= 0.0f)
    {
        timer = 1.0f * 1000;
        textAssets[0].isVisible = !textAssets[0].isVisible;
    }

    // Small state machine using stack collection
    switch (gameStages.top().game_stageID)
    {
    case GS_LOGO:
        GSLogoStateUpdate(deltaTime, resourceManager);
        break;
    case GS_MAIN_MENU:

        break;
    case GS_GAMEPLAY:
        break;
    case GS_INVALID:
    default:
        break;
    }
    // input paletas
    if (gameInputState.down2)
    {
        spritesAssets[indexPaddleBlueID].dest.y += 0.3f * deltaTime;
    }

    if (gameInputState.up2)
    {
        spritesAssets[indexPaddleBlueID].dest.y -= 0.3f * deltaTime;
    }

    if (gameInputState.down)
    {
        spritesAssets[indexPaddleRedID].dest.y += 0.3f * deltaTime;
    }

    if (gameInputState.up)
    {
        spritesAssets[indexPaddleRedID].dest.y -= 0.3f * deltaTime;
    }

    if (gameInputState.start && (puntoAzul >= 5 || puntoRojo >= 5))
    {
        puntoAzul = 0;
        puntoRojo = 0;
        restartCounter = true;
    }

    if (puntoAzul >= 5 || puntoRojo >= 5)
    {
        string fontfilePath = "assets/fonts/Blippo bold.ttf";
        TTF_Font *Sans = TTF_OpenFont(fontfilePath.c_str(), 24);
        SDL_Color Orange = {255, 165, 0};

        SDL_Surface *surfaceMessage = NULL;

        surfaceMessage = TTF_RenderText_Solid(Sans, "Press space to restart", Orange);

        SDL_Texture *Message_rect = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

        /* Text mainText;
        mainText.font = Sans;
        mainText.color = Orange;
        mainText.surface = surfaceMessage;
        mainText.texture = Message;
        mainText.dest = Message_rect; */

        textAssets[orangeTextID].surface = surfaceMessage;
        textAssets[orangeTextID].texture = Message_rect;
    }
    else if (restartCounter)
    {
        string fontfilePath = "assets/fonts/Blippo bold.ttf";
        TTF_Font *Sans = TTF_OpenFont(fontfilePath.c_str(), 24);
        SDL_Color Orange = {255, 165, 0};

        SDL_Surface *surfaceMessage = NULL;

        surfaceMessage = TTF_RenderText_Solid(Sans, "", Orange);

        SDL_Texture *Message_rect = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

        /* Text mainText;
        mainText.font = Sans;
        mainText.color = Orange;
        mainText.surface = surfaceMessage;
        mainText.texture = Message;
        mainText.dest = Message_rect; */

        textAssets[orangeTextID].surface = surfaceMessage;
        textAssets[orangeTextID].texture = Message_rect;
    }

    // rebote pelota
    spritesAssets[indexSpritePelotaID].dest.y += VelY * deltaTime;

    spritesAssets[indexSpritePelotaID].dest.x += VelX * deltaTime * acceleration;

    // colisión bordes superiores e inferiores
    if (spritesAssets[indexSpritePelotaID].dest.y > HEIGHT)
    {
        VelY = VelY * -1;
    }

    if (spritesAssets[indexSpritePelotaID].dest.y < 0)
    {
        VelY = VelY * -1;
    }

    // reset pelota lado rojo (derecha) y puntuación para azul
    if (spritesAssets[indexSpritePelotaID].dest.x > WIDTH * 1.038 || restartCounter)
    {
        if (!restartCounter)
        {
            puntoAzul++;
        }
        acceleration = 1;
        string fontfilePath = "assets/fonts/Blippo bold.ttf";
        TTF_Font *Sans = TTF_OpenFont(fontfilePath.c_str(), 24);
        SDL_Color Blue = {0, 0, 255};

        SDL_Surface *blueScoreMessage = NULL;

        string puntuacion = to_string(puntoAzul);

        const char *cstr = puntuacion.c_str();

        if (puntoAzul == 0)
        {
            blueScoreMessage = TTF_RenderText_Solid(Sans, "0", Blue);
        }

        if (puntoAzul == 1)
        {
            blueScoreMessage = TTF_RenderText_Solid(Sans, "1", Blue);
        }

        if (puntoAzul == 2)
        {
            blueScoreMessage = TTF_RenderText_Solid(Sans, "2", Blue);
        }

        if (puntoAzul == 3)
        {
            blueScoreMessage = TTF_RenderText_Solid(Sans, "3", Blue);
        }

        if (puntoAzul == 4)
        {
            blueScoreMessage = TTF_RenderText_Solid(Sans, "4", Blue);
        }

        if (puntoAzul >= 5)
        {
            blueScoreMessage = TTF_RenderText_Solid(Sans, "WIN", Blue);
        }

        if (puntoAzul < 5)
        {
            if (!restartCounter)
            {
                VelX = VelX * -1 * acceleration;
                string filePathBall = "assets/img/ball.png";
                SDL_Texture *texture2 = IMG_LoadTexture(renderer, filePathBall.c_str());
                SDL_assert(texture2 != NULL);
                SDL_Rect dest2;
                dest2.w = WIDTH * 0.05;                  // controls the width of the rect
                dest2.h = HEIGHT * 0.05;                 // controls the height of the rect
                dest2.x = (WIDTH >> 1) - (dest2.w >> 1); // controls the rect's x coordinate
                dest2.y = HEIGHT >> 1;                   // controls the rect's y coordinte

                Sprite ballSprite;
                ballSprite.dest = dest2;
                ballSprite.texture = texture2;
                spritesAssets.push_back(ballSprite);

                indexSpritePelotaID = spritesAssets.size() - 1;
            }
        }
        SDL_Texture *blueScore = SDL_CreateTextureFromSurface(renderer, blueScoreMessage);

        textAssets[blueTextID].surface = blueScoreMessage;
        textAssets[blueTextID].texture = blueScore;
    }

    // reset pelota lado azul (izquierda) y puntuación para rojo
    if (spritesAssets[indexSpritePelotaID].dest.x < -40 || restartCounter)
    {
        if (!restartCounter)
        {
            puntoRojo++;
        }
        acceleration = 1;
        string fontfilePath = "assets/fonts/Blippo bold.ttf";
        TTF_Font *Sans = TTF_OpenFont(fontfilePath.c_str(), 24);
        SDL_Color Red = {255, 0, 0};

        SDL_Surface *redScoreMessage = NULL;

        string puntuacion = to_string(puntoRojo);

        const char *cstr = puntuacion.c_str();

        if (puntoRojo == 0)
        {
            redScoreMessage = TTF_RenderText_Solid(Sans, "0", Red);
        }

        if (puntoRojo == 1)
        {
            redScoreMessage = TTF_RenderText_Solid(Sans, "1", Red);
        }

        if (puntoRojo == 2)
        {
            redScoreMessage = TTF_RenderText_Solid(Sans, "2", Red);
        }

        if (puntoRojo == 3)
        {
            redScoreMessage = TTF_RenderText_Solid(Sans, "3", Red);
        }

        if (puntoRojo == 4)
        {
            redScoreMessage = TTF_RenderText_Solid(Sans, "4", Red);
        }

        if (puntoRojo >= 5)
        {
            redScoreMessage = TTF_RenderText_Solid(Sans, "WIN", Red);
        }

        if (puntoRojo < 5)
        {
            VelX = VelX * -1 * acceleration;
            string filePathBall = "assets/img/ball.png";
            SDL_Texture *texture2 = IMG_LoadTexture(renderer, filePathBall.c_str());
            SDL_assert(texture2 != NULL);
            SDL_Rect dest2;
            dest2.w = WIDTH * 0.05;                  // controls the width of the rect
            dest2.h = HEIGHT * 0.05;                 // controls the height of the rect
            dest2.x = (WIDTH >> 1) - (dest2.w >> 1); // controls the rect's x coordinate
            dest2.y = HEIGHT >> 1;                   // controls the rect's y coordinte

            Sprite ballSprite;
            ballSprite.dest = dest2;
            ballSprite.texture = texture2;
            spritesAssets.push_back(ballSprite);

            indexSpritePelotaID = spritesAssets.size() - 1;
        }
        SDL_Texture *redScore = SDL_CreateTextureFromSurface(renderer, redScoreMessage);

        textAssets[redTextID].surface = redScoreMessage;
        textAssets[redTextID].texture = redScore;
        restartCounter = false;
    }

    // colisión con las paletas

    if (spritesAssets[indexSpritePelotaID].dest.x < spritesAssets[indexPaddleBlueID].dest.x + 40 && spritesAssets[indexSpritePelotaID].dest.x > spritesAssets[indexPaddleBlueID].dest.x)
    {
        if (spritesAssets[indexSpritePelotaID].dest.y > spritesAssets[indexPaddleBlueID].dest.y && spritesAssets[indexSpritePelotaID].dest.y < spritesAssets[indexPaddleBlueID].dest.y + HEIGHT * 0.33)
        {
            acceleration = acceleration + 0.1;
            VelX = 0.3 * acceleration;
        }
    }

    if (spritesAssets[indexSpritePelotaID].dest.x > spritesAssets[indexPaddleRedID].dest.x - 40 && spritesAssets[indexSpritePelotaID].dest.x < spritesAssets[indexPaddleRedID].dest.x)
    {
        if (spritesAssets[indexSpritePelotaID].dest.y > spritesAssets[indexPaddleRedID].dest.y && spritesAssets[indexSpritePelotaID].dest.y < spritesAssets[indexPaddleRedID].dest.y + HEIGHT * 0.33)
        {
            acceleration = acceleration + 0.1;
            VelX = -0.3 * acceleration;
        }
    }
}

void render()
{
    // Limpio la pantalla
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Pinto todos los sprites...
    for (int i = 0; i < spritesAssets.size(); i++)
    {
        if (spritesAssets[i].isVisible)
        {
            SDL_RenderCopy(renderer, spritesAssets[i].texture, NULL, &spritesAssets[i].dest);
        }
    }

    // Pinto todos los textos...
    for (int i = 0; i < textAssets.size(); i++)
    {
        if (textAssets[i].isVisible)
        {
            SDL_RenderCopy(renderer, textAssets[i].texture, NULL, &textAssets[i].dest);
        }
    }

    // Presento la imagen en pantalla
    SDL_RenderPresent(renderer);
}

///////// Funciones de actualizacion y pintado /////////////

///////// Funcion principal y GameLoop /////////////

int main(int argc, char *argv[])
{
    initEngine();

    // Cargo Assets
    loadAssets();

    Mix_PlayMusic(musicAssets[0].music, -1);

    Uint64 currentTime = SDL_GetTicks64();

    while (isGameRunning)
    {

        Uint64 previousTime = currentTime;

        currentTime = SDL_GetTicks64();

        Uint64 deltaTime = currentTime - previousTime;

        inputUpdate();

        updateGame(deltaTime * time_multiplier);

        render();
    }

    // Detendo la musica
    Mix_HaltMusic();

    // Descargo Assets
    unloadAssets();
    destroyEngine();
    return 0;
}

///////// Funcion principal y GameLoop /////////////

// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln
