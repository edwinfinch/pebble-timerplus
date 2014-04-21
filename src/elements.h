#define SETTINGS_KEY 0
#define NUM_MENU_SECTIONS 4
#define NUM_FIRST_MENU_ITEMS 5
#define NUM_SECOND_MENU_ITEMS 1
#define NUM_THIRD_MENU_ITEMS 1
#define NUM_FOURTH_MENU_ITEMS 3
	
typedef struct persist{
	uint8_t vibrate;
	bool theme;
	uint8_t hideActionBar;
	bool hideStatusBar;
	uint8_t hideUnused;
	uint8_t stopwatchTimer; 
	uint8_t animationSpeed;
}__attribute__((__packed__)) persist;

persist settings = {
	.vibrate = 1,
	.theme = 0,
	.hideActionBar = 0,
	.hideStatusBar = 0,
	.hideUnused = 0,
	.stopwatchTimer = 0,
	.animationSpeed = 0,
};

Window *window, *menu_window;
static PropertyAnimation *prop_animation;
/*
I know what you're thinking, it's probably something like: "holy TextLayers Batman". First of all, I'm not Batman. Abed is Batman now.
https://www.youtube.com/watch?v=uDIEND_DUuQ
Second of all, without this many text layers it would take a lot more time and a lot more code to make it as wonderful as it is now.
*/
TextLayer *min_text_layer, *sec_text_layer, *hour_text_layer, *des_text_layer, *timer_text_layer, *option_text_layer1, *option_text_layer2, *option_text_layer3;
TextLayer *stopwatch_text_layer1, *stopwatch_text_layer2, *stopwatch_text_layer3, *minutes_text_layer, *seconds_text_layer, *hours_text_layer, *lap_text_layer;
static GBitmap *actionIconPlus, *actionIconMinus, *actionIconNext, *actionIconCheckmark, *actionIconPause, *actionIconResume, *actionIconRestart, *actionIconStop;
static GBitmap *actionIconSettings, *actionIconTimer, *actionIconStopwatch, *actionIconLap;
InverterLayer *theme;


static SimpleMenuLayer *options;
static SimpleMenuSection menu_sections[4];
static SimpleMenuItem first_menu_items[5];
static SimpleMenuItem second_menu_items[1];
static SimpleMenuItem third_menu_items[1];
static SimpleMenuItem fourth_menu_items[3];

bool stopVibe, hideStatusBar;

int sw_seconds = 0;
int sw_minutes = 0;
int sw_hours = 0;

int lapSec, lapMin, lapHour;
double lap_time, now, elapsed_time, start_time;

typedef struct mytimer_struct {
    int16_t min;
    int16_t sec;
	int16_t hour;
} timer;
static timer timerData;
GRect start, finish, start1, finish1, start2, finish2, start3, finish3;
static ActionBarLayer *main_action_bar;
int intervalHour = 0;
int intervalMin = 1;
int intervalSec = 0;
int desText, running, running_stopwatch, flash, ended, valueRead, valueWritten;
bool initialScreen = 1;
bool setHour, setMinute, setSecond, actionBarHidden;
int animationSpeed = 600;
char timeFormatMin[]="999";
char timeFormatSec[]="00";
char timerSec[]="00";
char timerMin[]="00";
char timerHour[]="00";
char stopwatchSec[]="00";
char stopwatchMin[]="00";
char stopwatchHour[]="00";
char timeFormatHour[]="00";
char lapBuffer[]="Lap: ~00:00:00";