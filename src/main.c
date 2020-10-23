/*
== SANS 84 ==
Experience the hell that is the Sans fight from the comfort of your math class!
Programed for a TI-84 Plus CE.

NOTE: THIS CODE IS TRASH!
I used this project to begin to learn C. As such, if you actually know what you
are doing, I advise you to cover your eyes in fear.
Then, submit a pull request and show me how to do it *right*.

How to Compile.
- TODO: GOOD LUCK

This uses a toolchain by the CE-Programing team, THANK YOU SO MUCH.
You can get the toolchain here: https://github.com/CE-Programming/toolchain

TODO:
- "Attack" Rendering loop.
- HP Display
- Settle on a proper way to queue up attacks.
- When using NumPad, the buttons do not "hold", but the dumb arrows do.
*/

#include <compression.h>
#include <graphx.h>
#include <tice.h>

#include <debug.h>

/* Include the converted graphics file */
#include "gfx/gfx.h"

/*=== Global Prototypes ===*/
void begin(void);
void end(void);
bool step(void);
void draw(void);
void dTextBox(const char *str, int ln);

/*=== Global Varibles ===*/
/*Tile Varibles*/
#define sH_TILE_WIDTH 32
#define sH_TILE_HEIGHT 30

#define sB_TILE_WIDTH 75
#define sB_TILE_HEIGHT 49

#define sL_TILE_WIDTH 49
#define sL_TILE_HEIGHT 21

#define p_HEART_TILE_WIDTH 13
#define p_HEART_TILE_HEIGHT 10

#define BUTTON_TILE_WIDTH 73
#define BUTTON_TILE_HEIGHT 28

/*BoneArray*/
#define sBonesMax 20 // Maximum # of bones on screen at oneTime
#define sBonesY 6    //

/*Game States*/
#define TITLE_STATE 0
#define GAME_STATE 1

#define MAXHEALTH 100

#define compress

gfx_sprite_t *sH[16]; // Sans Head
int sFState = 0;
gfx_sprite_t *sB[10]; // Sans Body
int sBState = 0;
gfx_sprite_t *sL[2]; // Sans Legs
int sLState = 0;

gfx_sprite_t *pHeart[2];
int pHeartState = 1;

gfx_sprite_t *buttons[8]; // Buttons

gfx_sprite_t *tmp_ptr;

// int gameState = TITLE_STATE;
int gameState = GAME_STATE;

int phase;
int atk = 0; // Is sans in Attack? Big or small play area?

// int sScaleFactor = 1; // Sans Scale Factor

int sHX, sHY, sBX, sBY, sLX, sLY;
int rX, rY, rW, rH;
int sScaleFactor = 1;
int i = 0;

// EVERYTHING FOR PLAYER
int pHeartLocation[2] = {0, 0};

int health = MAXHEALTH;

int activeButton = 1;

bool sAnim = true;  // To be made redundant.
int sAnimClock = 0; // Constantly moving upwards.
int sAnimFrame = 0; // Current frame of animation.
int sAnimTarget =
    0; // Is equal to current sAnimClock + Delay. Set when frame plays.

/* Stores Base Sans Location, and any anim offsets.
Theese are the values modified by animation logic to actually move the sprites
around. */
int sansAnimOffset[8] = {65, 0,  // Base X,Y
                         0,  0,  // Head Anim Offset
                         0,  0,  // Body Anim Offset
                         0,  0}; // Legs Body Offset

int playArea[3] = {10, 96, 112}; // X,Y, Height Down.

/* Stores active Bones to be rendered.
-- Static, should not change during lifetime of bone.
    Bone Active?
    Direction of travel: 0, 1, 2, 3. Orientation automatically selected. 0, N to
S, 1, E to W, etc. Speed (PX per refresh) Lenth from corner of screen. Bint to
Top or Bottom? Blue? Time to next bone

-- Changes
    Current X,Y
          ↓↓ THIS IS THE MAXIMUM NUMBER OF BONES ON SCREEN AT A TIME. */
int sBones[sBonesMax][sBonesY] = {1, 0, 0, 0, 0, 0, 0, 0};
char textInt[2] = {55, 55};
// char textChar[1] = {"do you want to have a bad time?"};
// char textChar = "do you want to have a bad time?";
/* === CONSTANTS. STATIC, NOT MODIFIED BY PROGRAM!!!  === */

/* ANIMATIONS */

/* Idle Animation.
Each "frame" is stored as modifications from the default location. */
const unsigned int sAnim_IDLE[13][13] = {
    {
        5, 2,    // Delay to next frame.
        108, 5,  // Master XY
        0, 0, 0, // hXY, State(texture)
        0, 0, 0, // bXY, Texture
        0, 0, 0  // lXY, texture
    },
    {
        5, 2,     // Delay since last frame, Scale Factor,
        108, 5,   // Master XY
        0, -1, 0, // hXY, State(texture)
        0, -1, 0, // bXY, Texture
        0, 0, 0   // lXY, texture
    },
    {
        5, 2,     // Delay since last frame, Scale Factor,
        108, 5,   // Master XY
        0, -2, 0, // hXY, State(texture)
        0, -1, 0, // bXY, Texture
        0, 0, 0   // lXY, texture
    },
    {
        5, 2,     // Delay since last frame, Scale Factor,
        108, 5,   // Master XY
        0, -2, 0, // hXY, State(texture)
        0, -2, 0, // bXY, Texture
        0, 0, 0   // lXY, texture
    },
    {
        5, 2,      // Delay since last frame, Scale Factor,
        108, 5,    // Master XY
        -1, -2, 0, // hXY, State(texture)
        -1, -1, 0, // bXY, Texture
        0, 0, 0    // lXY, texture
    },
    {
        5, 2,      // Delay since last frame, Scale Factor,
        108, 5,    // Master XY
        -1, -1, 0, // hXY, State(texture)
        -1, -1, 0, // bXY, Texture
        0, 0, 0    // lXY, texture
    },
    {
        5, 2,      // Delay since last frame, Scale Factor,
        108, 5,    // Master XY
        -1, 0, 0,  // hXY, State(texture)
        -1, -1, 0, // bXY, Texture
        0, 0, 0    // lXY, texture
    },
    {
        5, 2,     // Delay since last frame, Scale Factor,
        108, 5,   // Master XY
        -2, 0, 0, // hXY, State(texture)
        -2, 0, 0, // bXY, Texture
        0, 0, 0   // lXY, texture
    },
    {
        5, 2,      // Delay since last frame, Scale Factor,
        108, 5,    // Master XY
        -2, -1, 0, // hXY, State(texture)
        -2, -1, 0, // bXY, Texture
        0, 0, 0    // lXY, texture
    },
    {
        5, 2,      // Delay since last frame, Scale Factor,
        108, 5,    // Master XY
        -2, -2, 0, // hXY, State(texture)
        -2, -1, 0, // bXY, Texture
        0, 0, 0    // lXY, texture
    },
    {
        5, 2,      // Delay since last frame, Scale Factor,
        108, 5,    // Master XY
        -2, -2, 0, // hXY, State(texture)
        -2, -2, 0, // bXY, Texture
        0, 0, 0    // lXY, texture
    },
    {
        5, 2,      // Delay since last frame, Scale Factor,
        108, 5,    // Master XY
        -1, -2, 0, // hXY, State(texture)
        -1, -1, 0, // bXY, Texture
        0, 0, 0    // lXY, texture
    },
    {
        5, 2,      // Delay since last frame, Scale Factor,
        108, 5,    // Master XY
        -1, -1, 0, // hXY, State(texture)
        -1, -1, 0, // bXY, Texture
        0, 0, 0    // lXY, texture
    },
};

/* Head Offset array. */
const int hOffsetConst[2] = {10, 0};

/* Body Offset array
    Offset from base when using different body sprite.*/
const unsigned int bOffsetConst[20] = {
    0, 24, 0, 24, 0, 24, 1, 24, -9, 24, // Some kind of snafu with scaling here.
    0, 24, 0, 24, 0, 24, 0, 6,  0,  8};

/* Leg Offset array. */
const int lOffsetConst[2] = {2, 50};

/* ATTACKS */
#define ATK_INTRO 1
/*
#define ATK_ 2
#define ATK_ 3
#define ATK_ 4
#define ATK_ 5
#define ATK_ 6
#define ATK_ 7
#define ATK_ 8
#define ATK_ 9
#define ATK_ 10
#define ATK_ 11
#define ATK_ 12
#define ATK_ 13
#define ATK_ 14
#define ATK_ 15
#define ATK_ 16
#define ATK_ 17
#define ATK_ 18
#define ATK_ 19
#define ATK_ 20
*/

/* Stores all attacks in a sequence */
const int attacks[1][1] = {{4}};

int main(void) {
  bool partial_redraw = false;

  /* No rendering allowed! */
  begin();

  /* Initialize graphics drawing */
  gfx_Begin();

  /* Draw to the buffer to avoid rendering artifacts */
  gfx_SetDrawBuffer();

  /* No rendering allowed in step! */
  while (step()) {
    /* Only want to redraw part of the previous frame? */
    if (partial_redraw) {
      /* Copy previous frame as a base for this frame */
      gfx_BlitScreen();
    }

    /* As little non-rendering logic as possible */
    draw();

    /* Queue the buffered frame to be displayed */
    gfx_SwapDraw();
  }

  /* End graphics drawing */
  // TODO: Free up memory on end of execution.
  free(sH);
  gfx_End();
  end();

  return 0;
}

/* Run Once when the program starts */
void begin(void) {
  unsigned int i;
  // Decompress, Load sprites into memory
  // SandHead

  // SandHead
  tmp_ptr = 0;
  for (i = 0; i < sizeof(t_heads_tiles_compressed) / sizeof(gfx_sprite_t *);
       ++i) {
    tmp_ptr = gfx_MallocSprite(sH_TILE_WIDTH, sH_TILE_HEIGHT);

    zx7_Decompress(tmp_ptr, t_heads_tiles_compressed[i]);
    sH[i] = tmp_ptr;
  }
  tmp_ptr = 0; // Idk if this does anything. There seems to be some kind of
               // interference, will test it's removal.
  for (i = 0; i < sizeof(t_body_tiles_compressed) / sizeof(gfx_sprite_t *);
       ++i) {
    tmp_ptr = gfx_MallocSprite(sB_TILE_WIDTH, sB_TILE_HEIGHT);

    zx7_Decompress(tmp_ptr, t_body_tiles_compressed[i]);
    sB[i] = tmp_ptr;
  }
  tmp_ptr = 0;
  for (i = 0; i < sizeof(t_legs_tiles_compressed) / sizeof(gfx_sprite_t *);
       ++i) {
    tmp_ptr = gfx_MallocSprite(sL_TILE_WIDTH, sL_TILE_HEIGHT);

    zx7_Decompress(tmp_ptr, t_legs_tiles_compressed[i]);
    sL[i] = tmp_ptr;
  }
  tmp_ptr = 0;
  for (i = 0; i < sizeof(buttons_tiles_compressed) / sizeof(gfx_sprite_t *);
       ++i) {
    tmp_ptr = gfx_MallocSprite(BUTTON_TILE_WIDTH, BUTTON_TILE_HEIGHT);

    zx7_Decompress(tmp_ptr, buttons_tiles_compressed[i]);
    buttons[i] = tmp_ptr;
  }
  tmp_ptr = 0;
  for (i = 0; i < sizeof(p_heart_tiles_compressed) / sizeof(gfx_sprite_t *);
       ++i) {
    tmp_ptr = gfx_MallocSprite(p_HEART_TILE_HEIGHT, p_HEART_TILE_WIDTH);

    zx7_Decompress(tmp_ptr, p_heart_tiles_compressed[i]);
    pHeart[i] = tmp_ptr;
  }

  gfx_SetTextFGColor(2);
  // gfx_SetTextScale(2, 2);
  // gfx_SetMonospaceFont(14);
}

/* Implement me! */
void end(void) {}

/* Implement me! */
bool step(void) {
  // Logic
  sk_key_t key;
  key = os_GetCSC();

  // Default Anim Logic.
  if (sAnim) {

    if (sAnimClock == sAnimTarget) { // Frame Logic.
      // Set everything to current frame.
      switch (atk) {
      case 0: // Normal Mode
        sScaleFactor = 2;
        sansAnimOffset[0] = 108;
        sansAnimOffset[1] = 5;
        break;

      case 1: // Attack

        sScaleFactor = 1;
        sansAnimOffset[0] = 130;
        sansAnimOffset[1] = 5;
        break;
      default:
        break;
      }

      sansAnimOffset[2] = sAnim_IDLE[sAnimFrame][4];
      sansAnimOffset[3] = sAnim_IDLE[sAnimFrame][5];
      sansAnimOffset[4] = sAnim_IDLE[sAnimFrame][7];
      sansAnimOffset[5] = sAnim_IDLE[sAnimFrame][8];
      sansAnimOffset[6] = sAnim_IDLE[sAnimFrame][10];
      sansAnimOffset[7] = sAnim_IDLE[sAnimFrame][11];

      sFState = sAnim_IDLE[sAnimFrame][6];
      sBState = sAnim_IDLE[sAnimFrame][9];
      sBState = sAnim_IDLE[sAnimFrame][12];

      if (sizeof(sAnim_IDLE) - 39 !=
          39 * (sAnimFrame)) { // TODO: Only run if sure that next frame is NOT
                               // last frame.
        sAnimTarget = sAnimClock + sAnim_IDLE[sAnimFrame][0];
        sAnimFrame++;
      } else { // TODO: Run if last frame. Resets everything

        sAnimFrame = 0;
        sAnimClock = 0;
        sAnimTarget = sAnimClock + sAnim_IDLE[sAnimFrame][0];
      }
    }
    sAnimClock++;
    // dbg_SetWatchpoint(&sAnimClock, sizeof sAnimClock);

    // Stuff to do Regardless
    switch (atk) {
    case 0: // Normal Mode
      // playArea = {10, 96, 112};   // I wish I could do it like this but alas.
      // Idk how.
      playArea[0] = 10;
      playArea[1] = 156;
      playArea[2] = 40;

      if (activeButton != 0) { // Heart in Menu
        pHeartLocation[1] = 211;
        switch (activeButton) {
        case 1:
          pHeartLocation[0] = 11;
          break;
        case 2:
          pHeartLocation[0] = 87;
          break;
        case 3:
          pHeartLocation[0] = 161;
          break;
        case 4:
          pHeartLocation[0] = 235;
          break;
        default:
          break;
        }
      }

      break;

    case 1: // Attack
      playArea[0] = 10;
      playArea[1] = 96;
      playArea[2] = 112;

      if (pHeartLocation[0] < playArea[0]) {
        pHeartLocation[0] = playArea[0];
      }
      if (pHeartLocation[0] > gfx_lcdWidth - playArea[0]-p_HEART_TILE_WIDTH) {
        pHeartLocation[0] = gfx_lcdWidth - playArea[0]-p_HEART_TILE_WIDTH;
      }
        if (pHeartLocation[1] < playArea[1]) {
        pHeartLocation[1] = playArea[1];
      }
      if (pHeartLocation[1] > playArea[1]+playArea[2]-p_HEART_TILE_HEIGHT) {
        pHeartLocation[1] = playArea[1]+playArea[2]-p_HEART_TILE_HEIGHT;
      }
      break;
    default:
      break;
    }
  }

  switch (atk) {
  case 0:
    switch (key) {
    case sk_Down:
    case sk_2:

      return true;
    case sk_Left:
    case sk_4:
      if (activeButton > 1) {
        activeButton--;
      }
      return true;
    case sk_Right:
    case sk_6:
      if (activeButton < 4) {
        activeButton++;
      }
      return true;
    case sk_Up:
    case sk_8:

      return true;
    case sk_RParen:

      return true;
    case sk_LParen:

      return true;
    case sk_Sub:
      return true;
    case sk_Enter: // End Program
      return false;

    case sk_1:
      if (atk > 0) {
        atk--;
      }
      return true;
    case sk_3:
      if (atk < 1) {
        atk++;
        pHeartLocation[0] = 200;
        pHeartLocation[1] = (playArea[1] + playArea[2]) / 2;
      }
      return true;

    default:
      return true;
    }
    break;

  case 1:

    switch (key) {
    case sk_Down:
    case sk_2:
      pHeartLocation[1] += 10;
      return true;
    case sk_Left:
    case sk_4:
      pHeartLocation[0] -= 10;
      return true;
    case sk_Right:
    case sk_6:
      pHeartLocation[0] += 10;
      return true;
    case sk_Up:
    case sk_8:
      pHeartLocation[1] -= 10;
      return true;
    case sk_RParen:

      return true;
    case sk_LParen:

      return true;
    case sk_Sub:
      return true;
    case sk_Enter: // End Program
      return false;

    case sk_1:
      if (atk > 0) {
        atk--;
      }
      return true;
    case sk_3:
      if (atk < 1) {
        atk++;
        pHeartLocation[0] = 200;
        pHeartLocation[1] = (playArea[1] + playArea[2]) / 2;
      }
      return true;

    default:
      return true;
    }
    break;

  default:
    break;
  }
}

/* All the Draw Logic */
void draw(void) {
  gfx_SetPalette(global_palette, sizeof_global_palette, 0);
  gfx_SetTransparentColor(0);
  gfx_FillScreen(1);

  switch (gameState) {
  case TITLE_STATE:

    break;
  case GAME_STATE:
    // Draw Menu
    gfx_SetColor(2);

    gfx_FillRectangle_NoClip(playArea[0], playArea[1],
                             (320 - (2 * playArea[0])), playArea[2]);
    gfx_SetColor(1);

    gfx_FillRectangle_NoClip(playArea[0] + 4, playArea[1] + 4,
                             (320 - (2 * playArea[0])) - 8, playArea[2] - 8);

    // Draw Bones

    for (i = 0; i < sBonesMax; i++) {
      if (sBones[i][0] == 1) {
        // dbg_sprintf(dbgout, "Sbones = 1 = %d\n", i);
      } else {
        // dbg_sprintf(dbgerr, "Sbones = 0 = %d\n", i);
      }
    }

    // Draw Sans
    sHX = sansAnimOffset[0] + (sansAnimOffset[2] * sScaleFactor) +
          (hOffsetConst[0] * sScaleFactor);
    sHY = sansAnimOffset[1] + (sansAnimOffset[3] * sScaleFactor) +
          (hOffsetConst[1] * sScaleFactor);

    sBX = sansAnimOffset[0] + (sansAnimOffset[4] * sScaleFactor) +
          (bOffsetConst[sBState * 2] * sScaleFactor);
    sBY = sansAnimOffset[1] + (sansAnimOffset[5] * sScaleFactor) +
          (bOffsetConst[(sBState * 2) + 1] * sScaleFactor);

    sLX = sansAnimOffset[0] + (sansAnimOffset[6] * sScaleFactor) +
          (lOffsetConst[0] * sScaleFactor);
    sLY = sansAnimOffset[1] + (sansAnimOffset[7] * sScaleFactor) +
          (lOffsetConst[1] * sScaleFactor);

    gfx_ScaledTransparentSprite_NoClip(sH[sFState], sHX, sHY, sScaleFactor,
                                       sScaleFactor);
    gfx_ScaledTransparentSprite_NoClip(sB[sBState], sBX, sBY, sScaleFactor,
                                       sScaleFactor);
    gfx_ScaledTransparentSprite_NoClip(sL[sLState], sLX, sLY, sScaleFactor,
                                       sScaleFactor);

    dTextBox("* Do you want to have a bad time?", 0);
    // dTextBox("    bad time?", 1);

    switch (atk) {
    case 0:
      switch (activeButton) {
      case 0:
        gfx_Sprite(buttons[0], 10, 205);
        gfx_Sprite(buttons[1], 85, 205);
        gfx_Sprite(buttons[2], 160, 205);
        gfx_Sprite(buttons[3], 235, 205);
        break;
      case 1:
        gfx_Sprite(buttons[0 + 4], 10, 205);
        gfx_Sprite(buttons[1], 85, 205);
        gfx_Sprite(buttons[2], 160, 205);
        gfx_Sprite(buttons[3], 235, 205);
        break;
      case 2:
        gfx_Sprite(buttons[0], 10, 205);
        gfx_Sprite(buttons[1 + 4], 85, 205);
        gfx_Sprite(buttons[2], 160, 205);
        gfx_Sprite(buttons[3], 235, 205);
        break;
      case 3:
        gfx_Sprite(buttons[0], 10, 205);
        gfx_Sprite(buttons[1], 85, 205);
        gfx_Sprite(buttons[2 + 4], 160, 205);
        gfx_Sprite(buttons[3], 235, 205);
        break;
      case 4:
        gfx_Sprite(buttons[0], 10, 205);
        gfx_Sprite(buttons[1], 85, 205);
        gfx_Sprite(buttons[2], 160, 205);
        gfx_Sprite(buttons[3 + 4], 235, 205);
        break;
      }

    case 1:
      // UI when in Fight.
      break;
    default:
      break;
    }

    gfx_TransparentSprite(pHeart[pHeartState], pHeartLocation[0],
                          pHeartLocation[1]);
  default:
    break;
  }
}

// Utility
void dTextBox(const char *str, int ln) {
  switch (atk) {
  case 0: // Normal Mode
    gfx_PrintStringXY(str, 20, 165 + (12 * ln));

    break;
  case 2: // Attack
    gfx_PrintStringXY(str, 20, 106 + (12 * ln));
    break;
  default:
    break;
  }
}