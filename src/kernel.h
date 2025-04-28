// Constants
#define NUM_LEVELS 6
#define MAX_WIDTH 10
#define MAX_HEIGHT 10

#define EMPTY 0
#define WALL 1
#define BLOCK 2
#define SUCCESS_BLOCK 3
#define END_POINT 4
#define PLAYER 5
#define WATER 6
#define TRANSITION_LEVEL NUM_LEVELS - 1

// Level struct
typedef struct {
    int width;
    int height;
    char *solution;
    int map[MAX_WIDTH][MAX_HEIGHT];
} Level;

// Initialize the levels
Level levels[NUM_LEVELS] = {
    {7,6,"wasaawdddwdssaaasd", {{EMPTY, EMPTY, EMPTY, WALL, WALL, WALL, WALL},{ WALL, WALL, WALL, WALL, EMPTY, EMPTY, WALL},{WALL, EMPTY, BLOCK, EMPTY, EMPTY, EMPTY, WALL},{WALL, EMPTY, EMPTY, BLOCK, PLAYER, EMPTY, WALL},{WALL, WALL, EMPTY, EMPTY, END_POINT, END_POINT, WALL},{EMPTY, WALL, WALL, WALL, WALL, WALL, WALL}}},
    {7,6,"ddsassddwaadddwasaawwds", {{WALL, WALL, WALL, WALL, WALL, EMPTY, EMPTY },{ WALL, PLAYER, EMPTY, EMPTY, WALL, WALL, WALL},{WALL, EMPTY, EMPTY, BLOCK, BLOCK, EMPTY, WALL},{WALL, END_POINT, EMPTY, END_POINT, EMPTY, EMPTY, WALL},{WALL, WALL, EMPTY, EMPTY, EMPTY, WALL, WALL},{EMPTY, WALL, WALL, WALL, WALL, WALL, EMPTY}}},
    {7,6,"wdwassaaawdsddwwwwaassasd",{{ EMPTY, WALL, WALL, WALL, WALL, WALL, EMPTY },{EMPTY, WALL, EMPTY, EMPTY, EMPTY, WALL, WALL},{WALL, WALL, BLOCK, END_POINT, EMPTY, EMPTY, WALL},{WALL, EMPTY, BLOCK, END_POINT, BLOCK, EMPTY, WALL},{WALL, EMPTY, EMPTY, END_POINT, PLAYER, WALL, WALL},{WALL, WALL, WALL, WALL, WALL, WALL, EMPTY}}},
    {8,10, "sasswwddddssassddwadwawwawwasdsassdw",{{EMPTY, EMPTY, WALL, WALL, WALL, WALL, EMPTY, EMPTY},{EMPTY, WALL, WALL, EMPTY, EMPTY, WALL, EMPTY, EMPTY},{WALL, WALL, PLAYER, BLOCK, EMPTY, WALL, WALL, EMPTY},{WALL, EMPTY, END_POINT, EMPTY, END_POINT, EMPTY, WALL, WALL},{WALL, BLOCK, WALL, EMPTY, BLOCK, BLOCK, END_POINT, WALL},{WALL, EMPTY, EMPTY, BLOCK, EMPTY, EMPTY, BLOCK, WALL},{WALL, END_POINT, EMPTY, END_POINT, END_POINT, EMPTY, EMPTY, WALL},{WALL, WALL, WALL, EMPTY, EMPTY, EMPTY, EMPTY, WALL},{EMPTY, EMPTY, WALL, EMPTY, EMPTY, WALL, WALL, WALL},{EMPTY, EMPTY, WALL, WALL, WALL, WALL, EMPTY, EMPTY}}},
    {9,8, "wddssdsdsdwwsaawaasdsddaww", {{EMPTY, EMPTY, WALL, WALL, WALL, WALL, EMPTY, EMPTY, EMPTY},{WALL, WALL, WALL, EMPTY, EMPTY, WALL, WALL, WALL, WALL},{WALL, EMPTY, EMPTY, BLOCK, END_POINT, EMPTY, EMPTY, EMPTY, WALL},{WALL, PLAYER, WALL, EMPTY, END_POINT, WALL, END_POINT, EMPTY, WALL},{WALL, WALL, EMPTY, EMPTY, EMPTY, WATER, EMPTY, EMPTY, WALL},{EMPTY, WALL, EMPTY, BLOCK, BLOCK, EMPTY, BLOCK, WALL, WALL},{EMPTY, WALL, WALL, EMPTY, EMPTY, EMPTY, END_POINT, WALL, EMPTY},{EMPTY, EMPTY, WALL, WALL, WALL, WALL, WALL, WALL, EMPTY}}},
    {10, 3, "dddddddd", {{WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}, {PLAYER, BLOCK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, END_POINT}, {WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL}}}
};

// A copy of all levels
Level initialLevels[NUM_LEVELS];

// Functions
void drawMap(int rows, int cols, int map[MAX_WIDTH][MAX_HEIGHT], const unsigned long *allArray[7], int screenWidth, int screenHeight);
void movePlayer(int mapHeight, int mapWidth, int map[mapHeight][mapWidth], int direction);
int isLevelComplete(int mapHeight, int mapWidth, int map[mapHeight][mapWidth]);
void welcome_screen_display();
void instruction_screen_display();
void reset_levels();
void end_screen_display();
void displayRestartMessage();
