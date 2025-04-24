#include "./util.h"

#define BLACK_TEXT "\033[30m"
#define BLACK_BACKGROUND "\033[40m"

#define RED_TEXT "\033[31m"
#define RED_BACKGROUND "\033[41m"

#define YELLOW_TEXT "\033[33m"
#define YELLOW_BACKGROUND "\033[43m"

#define BLUE_TEXT "\033[34m"
#define BLUE_BACKGROUND "\033[44m"

#define PURPLE_TEXT "\033[35m"
#define PURPLE_BACKGROUND "\033[45m"

#define CYAN_TEXT "\033[36m"
#define CYAN_BACKGROUND "\033[46m"

#define WHITE_TEXT "\033[37m"
#define WHITE_BACKGROUND "\033[47m"

int change_color(char *text, char *background);