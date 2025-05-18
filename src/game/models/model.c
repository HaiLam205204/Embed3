// Define boolean type
#define TRUE 1
#define FALSE 0

// Constants
#define MAX_NAME_LENGTH 32
#define MAX_SKILLS_PER_PERSONA 6
#define MAX_SKILLS_PER_ENEMY 2

// Skill structure
typedef struct {
    char name[MAX_NAME_LENGTH];
    int is_aoe;         // 0 = single target, 1 = AOE
    int base_damage;
} Skill;

// Persona structure
typedef struct {
    char name[MAX_NAME_LENGTH];
} Persona;

// Attack structure
typedef struct {
    int base_damage;
} Attack;

// Run structure
typedef struct {
    int is_running;
} Run;

// Item structure
typedef struct {
    int item_quantitty;
    char name[MAX_NAME_LENGTH];
} Item;


// Action structure
typedef struct {
    Attack attack;
    Item item;
    Persona persona;
    Skill skill;
    Run run;
} Action;

// Character structure
typedef struct {
    char name[MAX_NAME_LENGTH];
    int is_main_character;
    int current_hp;
    int max_hp;
    Action action;
} Character;

// Enemy structure
typedef struct {
    char name[MAX_NAME_LENGTH];
    int max_hp;
    int current_hp;
    Skill skills[MAX_SKILLS_PER_ENEMY];
} Enemy;