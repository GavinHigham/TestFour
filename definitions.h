//#define ROOT
#define SCREEN_W 800
#define SCREEN_H 600
#define MARGIN 300
#define FPS 60
#define PROJ_POOL_SIZE 1000
#define ENEMY_POOL_SIZE 100
#define SHOT_COOLDOWN 8
#define ENEMY_COOLDOWN 2
#define SHOT_SPREAD 0
#define SHOT_OFFSET_X 69
#define SHOT_OFFSET_Y 20
#define BACKDROP_H 800
#define BACKDROP_W 1280
#define STARNUMBER 200
#define COLLISIONDIVISION 9

//A 2D floating-point vector.
typedef struct vector {
	float x;
	float y;
} VECTOR, *VP;

//A 2D integer vector.
typedef struct ivector {
	int x;
	int y;
} IVECT, *IVP;

typedef struct projectile {
	VP pos;
	VP vel;
	VP size;
	//void *next; //Used for collision.
	int kind;
	int animFrame;
} PROJ, *PROJP;

//"Kind" will be an integer, but I enumerate its possible values here.
enum {SHIP, BOLT, ASTEROID};

typedef struct smartpool {
	PROJP *pool;
	int poolsize;
	int liveIndex;
	int colgroup;
} SMARTPOOL, *SPP;

VECTOR struct_zero_vector = {0, 0};
VP zero_vec = &struct_zero_vector;
int numcolgroups = 0;

int i;
short fps = 0;
short logicTick = 0;
//Standard velocity.
float stdv = 3.1;
//Value to scale x and y vel by if both keys are pressed.
float diagscale = 0.71;
int backdropx = 0;
int backdropy = (SCREEN_H - BACKDROP_H) / 2;

ALLEGRO_DISPLAY *display   = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;

bool redraw = true;
bool doexit = false;

short boltFrame = 0;
ALLEGRO_BITMAP *bolt[12];
ALLEGRO_BITMAP *asteroid[60];

int shipFrame = 29;
int shipFramesetSwap = 60;
ALLEGRO_BITMAP *shipFrames[120];

PROJP ship;
int ship_cooldown = SHOT_COOLDOWN;
int enemy_cooldown = ENEMY_COOLDOWN;

SPP sl_pool;
SPP en_pool;

enum {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE};
bool key[5] = { false, false, false, false };

PROJP collision_array[8 * 6] = {0};