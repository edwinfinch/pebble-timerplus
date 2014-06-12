/*
Timer Plus
Sorry for the super messy code, this is mainly a hack created on lunch breaks and some home-time ;)
By: Edwin Finch
Under MIT license :) 

Extra credits:
Animated action bars from pedrolane (big thanks!): https://github.com/gregoiresage/pebble_animated_actionbar
Idea credit to a resturant napkin and a late night hackathon

#savegreendale
#sixseasonsandamovie
*/

#include <pebble.h>
#include "elements.h"

void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay);
void tick_handler(struct tm *tick_time, TimeUnits units_changed);	
void window_load_menu(Window *menu_window);
void window_unload_menu(Window *menu_window);
void tick_handler_stopwatch(struct tm *tick_time, TimeUnits units_changed);
void updateMin();
void updateHour();
void updateSec();
double float_time_ms() {
	time_t seconds;
	uint16_t milliseconds;
	time_ms(&seconds, &milliseconds);
	return (double)seconds + ((double)milliseconds / 1000.0);
}

//Separate function to update action bar icons, much cleaner
void updateActionBar(int set){
	if(set == 1){
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, actionIconTimer);
  		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_DOWN, actionIconStopwatch);
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_SELECT, actionIconSettings);
	}
	else if(set == 2){
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, actionIconPlus);
  		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_DOWN, actionIconMinus);
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_SELECT, actionIconNext);
	}
	else if(set == 3){
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, actionIconPause);
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_DOWN, actionIconStop);
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_SELECT, actionIconRestart);
	}
	else if(set == 4){
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, actionIconPause);
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_DOWN, actionIconLap);
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_SELECT, actionIconRestart);
	}
}

//Running an outdated version of animation action bar, so these are here with no current purpose
static void animation_started(Animation *animation, void *data) {
  
}

static void animation_stopped(Animation *animation, bool finished, void *data) {

}

//Destroy the animation
static void destroy_property_animation(PropertyAnimation **prop_animation) {
	//If it's null
  if (*prop_animation == NULL) {
	  //return
    return;
  }

	//if an animation is scheduled, unschedule it
  if (animation_is_scheduled((Animation*) *prop_animation)) {
    animation_unschedule((Animation*) *prop_animation);
  }

  property_animation_destroy(*prop_animation);
  *prop_animation = NULL;
}

//Where the magic happens
void actionBarAnimate(int i){
	GRect to_rect = layer_get_bounds(action_bar_layer_get_layer(main_action_bar));
	int delay = 3000;
	if(i == 1){
		//In
		to_rect.origin.y = 3;
      	to_rect.origin.x = 144;
		actionBarHidden = 1;
		delay = 3000;
	}
	else if(i == 2){
		//Out (show)
		to_rect.origin.y = 3;
      	to_rect.origin.x = 144 - 20;
		actionBarHidden = 0;
		delay = 100;
	}
		
		destroy_property_animation(&prop_animation);

  		prop_animation = property_animation_create_layer_frame(action_bar_layer_get_layer(main_action_bar), NULL, &to_rect);
  		animation_set_duration((Animation*) prop_animation, 400);
		animation_set_delay((Animation*) prop_animation, delay);
     	animation_set_curve((Animation*) prop_animation, AnimationCurveEaseOut);
		animation_set_handlers((Animation*) prop_animation, (AnimationHandlers) {
    		.started = (AnimationStartedHandler) animation_started,
    		.stopped = (AnimationStoppedHandler) animation_stopped,
        }, NULL /* callback data */);
  		animation_schedule((Animation*) prop_animation);
}

//Update the text of the description layer
void updateDesText(int i){
	if(i == 1){
		text_layer_set_text(des_text_layer, "Seconds");
	}
	else if(i == 3){
		text_layer_set_text(des_text_layer, "Paused");
	}
	else if(i == 4){
		text_layer_set_text(des_text_layer, "Ended");
	}
	else if(i == 5){
		text_layer_set_text(des_text_layer, "Minute");
	}
	else if(i == 6){
		text_layer_set_text(des_text_layer, "Hours");
	}
	else if(i == 7){
		text_layer_set_text(des_text_layer, "Second");
	}
	else if(i == 8){
		text_layer_set_text(des_text_layer, "Minutes");
	}
	else if(i == 9){
		text_layer_set_text(des_text_layer, "Hour");
	}
}

//When the time is up, vibrate according to the setting
void timeIsUp(){
	if(settings.vibrate == 1){
		vibes_short_pulse();
	}
	else if(settings.vibrate == 2){
		vibes_double_pulse();
	}
	else if(settings.vibrate == 3){
		vibes_long_pulse();
	}
	//Reshow the action bar
	actionBarAnimate(2);
}

void transition(int transNum, bool alt){
	//I'm sure there's a more efficient way to handle transitions but right now function > clean code
	//If alt is true, that means invert the transition. This is generally only used for backing out.
	if(transNum == 1){
		start = GRect(0, 40, 144, 168);
		finish = GRect(-120, 40, 144, 168);
		start1 = GRect(120, 40, 144, 168);
		finish1 = GRect(0, 40, 144, 168);
		start2 = GRect(0, 100, 144, 168);
		finish2 = GRect(-150, 100, 144, 168);
		start3 = GRect(300, 100, 144, 168);
		finish3 = GRect(0, 100, 144, 168);
		if(alt == 0){
			animate_layer(text_layer_get_layer(min_text_layer), &start, &finish, animationSpeed, 100);
			animate_layer(text_layer_get_layer(sec_text_layer), &start1, &finish1, animationSpeed, animationSpeed+100);
			animate_layer(text_layer_get_layer(des_text_layer), &start2, &finish2, animationSpeed, 200);
			animate_layer(text_layer_get_layer(des_text_layer), &start3, &finish3, animationSpeed, animationSpeed+200);
		}
		else{
			animate_layer(text_layer_get_layer(min_text_layer), &finish, &start, animationSpeed, 100);
			animate_layer(text_layer_get_layer(sec_text_layer), &finish1, &start1, animationSpeed, 200);
			animate_layer(text_layer_get_layer(des_text_layer), &start2, &finish2, animationSpeed, 200);
			animate_layer(text_layer_get_layer(des_text_layer), &start3, &finish3, animationSpeed, animationSpeed+200);
		}
	}
	else if(transNum == 2){
		start = GRect(0, 40, 144, 168);
		finish = GRect(-100, 40, 144, 168);
		if(alt == 0){
			animate_layer(text_layer_get_layer(sec_text_layer), &start, &finish, animationSpeed, 100);
		}
		else{
			animate_layer(text_layer_get_layer(sec_text_layer), &finish, &start, animationSpeed, 100);
		}
	}
	else if(transNum == 3){
		start2 = GRect(300, 100, 144, 168);
		finish2 = GRect(0, 100, 144, 168);
		if(alt == 0){
			animate_layer(text_layer_get_layer(des_text_layer), &start2, &finish2, animationSpeed, animationSpeed+100);
		}
		else{
			animate_layer(text_layer_get_layer(des_text_layer), &finish2, &start2, animationSpeed, animationSpeed+100);
		}
	}
	else if(transNum == 4){
		start = GRect(0, 40, 144, 168);
		finish = GRect(-120, 40, 144, 168);
		start1 = GRect(120, 40, 144, 168);
		finish1 = GRect(0, 40, 144, 168);
		start2 = GRect(0, 100, 144, 168);
		finish2 = GRect(-150, 100, 144, 168);
		start3 = GRect(300, 100, 144, 168);
		finish3 = GRect(0, 100, 144, 168);
		if(alt == 0){
			animate_layer(text_layer_get_layer(hour_text_layer), &start, &finish, animationSpeed, 100);
			animate_layer(text_layer_get_layer(min_text_layer), &start1, &finish1, animationSpeed, animationSpeed+100);
			animate_layer(text_layer_get_layer(des_text_layer), &start2, &finish2, animationSpeed, 200);
			animate_layer(text_layer_get_layer(des_text_layer), &start3, &finish3, animationSpeed, animationSpeed+200);
		}
		else{
			animate_layer(text_layer_get_layer(hour_text_layer), &finish, &start, animationSpeed, 100);
			animate_layer(text_layer_get_layer(min_text_layer), &finish1, &start1, animationSpeed, 200);
			animate_layer(text_layer_get_layer(des_text_layer), &start2, &finish2, animationSpeed, 200);
			animate_layer(text_layer_get_layer(des_text_layer), &start3, &finish3, animationSpeed, animationSpeed+200);
		}
	}
	else if(transNum == 5){
		start = GRect(0, 40, 144, 168);
		finish = GRect(0, -100, 144, 168);
		start1 = GRect(120, 40, 144, 168);
		finish1 = GRect(0, 40, 144, 168);
		start2 = GRect(0, 100, 144, 168);
		finish2 = GRect(-150, 100, 144, 168);
		start3 = GRect(300, 100, 144, 168);
		finish3 = GRect(0, 100, 144, 168);
		if(alt == 0){
			animate_layer(text_layer_get_layer(hour_text_layer), &start1, &finish1, animationSpeed, animationSpeed+100);
			animate_layer(text_layer_get_layer(des_text_layer), &start2, &finish2, animationSpeed, 200);
			animate_layer(text_layer_get_layer(des_text_layer), &start3, &finish3, animationSpeed, animationSpeed+200);
			animate_layer(text_layer_get_layer(timer_text_layer), &start, &finish, animationSpeed, 100);
		}
		else{
			animate_layer(text_layer_get_layer(hour_text_layer), &finish1, &start1, animationSpeed, animationSpeed+100);
			animate_layer(text_layer_get_layer(des_text_layer), &finish2, &start2, animationSpeed, 200);
			animate_layer(text_layer_get_layer(des_text_layer), &finish3, &start3, animationSpeed, animationSpeed+200);
			animate_layer(text_layer_get_layer(timer_text_layer), &finish, &start, animationSpeed, 100);
		}
	}
	else if(transNum == 6){
		finish = GRect(0, -50, 144, 168);
		start = GRect(20, 10, 144, 168);
		start1 = GRect(13, 55, 144, 168);
		start2 = GRect(-2, 103, 144, 168);
		GRect start11 = GRect(120, 40, 144, 168);
		GRect finish11 = GRect(0, 40, 144, 168);
		GRect start22 = GRect(-150, 100, 144, 168);
		GRect finish22 = GRect(0, 100, 144, 168);
		if(alt == 0){
			animate_layer(text_layer_get_layer(option_text_layer1), &start, &finish, animationSpeed, 100);
			animate_layer(text_layer_get_layer(option_text_layer2), &start1, &finish, animationSpeed, 200);
			animate_layer(text_layer_get_layer(option_text_layer3), &start2, &finish, animationSpeed, 300);
			animate_layer(text_layer_get_layer(hour_text_layer), &start11, &finish11, animationSpeed, 900);
			animate_layer(text_layer_get_layer(des_text_layer), &start22, &finish22, animationSpeed, 1000);
		}
		else{
			animate_layer(text_layer_get_layer(option_text_layer1), &finish, &start, animationSpeed, 800);
			animate_layer(text_layer_get_layer(option_text_layer2), &finish, &start1, animationSpeed, 900);
			animate_layer(text_layer_get_layer(option_text_layer3), &finish, &start2, animationSpeed, 1000);
			animate_layer(text_layer_get_layer(hour_text_layer), &finish11, &start11, animationSpeed, 100);
			animate_layer(text_layer_get_layer(des_text_layer), &finish22, &start22, animationSpeed, 200);
		}
	}
	else if(transNum == 7){
		finish = GRect(0, -50, 144, 168);
		start = GRect(20, 10, 144, 168);
		start1 = GRect(13, 55, 144, 168);
		start2 = GRect(-2, 103, 144, 168);
		if(alt == 0){
			animate_layer(text_layer_get_layer(option_text_layer1), &start, &finish, animationSpeed, 100);
			animate_layer(text_layer_get_layer(option_text_layer2), &start1, &finish, animationSpeed, 200);
			animate_layer(text_layer_get_layer(option_text_layer3), &start2, &finish, animationSpeed, 300);
		}
		else{
			animate_layer(text_layer_get_layer(option_text_layer1), &finish, &start, animationSpeed, 100);
			animate_layer(text_layer_get_layer(option_text_layer2), &finish, &start1, animationSpeed, 200);
			animate_layer(text_layer_get_layer(option_text_layer3), &finish, &start2, animationSpeed, 300);
		}
	}
	else if(transNum == 8){
		finish = GRect(0, -50, 144, 168);
		start = GRect(20, 10, 144, 168);
		start1 = GRect(13, 55, 144, 168);
		start2 = GRect(-2, 103, 144, 168);
		animate_layer(text_layer_get_layer(option_text_layer1), &start, &finish, animationSpeed, 100);
		animate_layer(text_layer_get_layer(option_text_layer2), &start1, &finish, animationSpeed, 200);
		animate_layer(text_layer_get_layer(option_text_layer3), &start2, &finish, animationSpeed, 300);
		start = GRect(0, -50, 144, 168);
		finish = GRect(27, 10, 144, 168);
		finish1 = GRect(27, 55, 144, 168);
		finish2 = GRect(27, 103, 144, 168);
		GRect finish11 = GRect(-35, 0, 144, 168);
		GRect finish12 = GRect(-35, 45, 144, 168);
		GRect finish13 = GRect(-35, 93, 144, 168);
		animate_layer(text_layer_get_layer(stopwatch_text_layer1), &start, &finish11, animationSpeed, 900);
		animate_layer(text_layer_get_layer(stopwatch_text_layer2), &start, &finish12, animationSpeed, 1000);
		animate_layer(text_layer_get_layer(stopwatch_text_layer3), &start, &finish13, animationSpeed, 1100);
		animate_layer(text_layer_get_layer(seconds_text_layer), &start, &finish, animationSpeed, 900);
		animate_layer(text_layer_get_layer(minutes_text_layer), &start, &finish1, animationSpeed, 1000);
		animate_layer(text_layer_get_layer(hours_text_layer), &start, &finish2, animationSpeed, 1100);
	}
	else if(transNum == 9){
		start = GRect(0, -50, 144, 168);
		finish = GRect(27, 10, 144, 168);
		finish1 = GRect(27, 55, 144, 168);
		finish2 = GRect(27, 103, 144, 168);
		GRect finish11 = GRect(-35, 0, 144, 168);
		GRect finish12 = GRect(-35, 45, 144, 168);
		GRect finish13 = GRect(-35, 93, 144, 168);
		if(alt == 0){
			animate_layer(text_layer_get_layer(stopwatch_text_layer1), &start, &finish11, animationSpeed, 600);
			animate_layer(text_layer_get_layer(stopwatch_text_layer2), &start, &finish12, animationSpeed, 700);
			animate_layer(text_layer_get_layer(stopwatch_text_layer3), &start, &finish13, animationSpeed, 800);
			animate_layer(text_layer_get_layer(seconds_text_layer), &start, &finish, animationSpeed, 700);
			animate_layer(text_layer_get_layer(minutes_text_layer), &start, &finish1, animationSpeed, 800);
			animate_layer(text_layer_get_layer(hours_text_layer), &start, &finish2, animationSpeed, 1100);
		}
		else{
			animate_layer(text_layer_get_layer(stopwatch_text_layer1), &finish11, &start, animationSpeed, 0);
			animate_layer(text_layer_get_layer(stopwatch_text_layer2), &finish12, &start, animationSpeed, 100);
			animate_layer(text_layer_get_layer(stopwatch_text_layer3), &finish13, &start, animationSpeed, 200);
			animate_layer(text_layer_get_layer(seconds_text_layer), &finish, &start, animationSpeed, 0);
			animate_layer(text_layer_get_layer(minutes_text_layer), &finish1, &start, animationSpeed, 100);
			animate_layer(text_layer_get_layer(hours_text_layer), &finish2, &start, animationSpeed, 200);
		}
	}
	else if(transNum == 10){
		start = GRect(0, 200, 144, 40);
		finish = GRect(0, 100, 144, 40);
		animate_layer(text_layer_get_layer(lap_text_layer), &start, &finish, animationSpeed, 0);
		animate_layer(text_layer_get_layer(lap_text_layer), &finish, &start, animationSpeed, 5000);
	}
}

//If back button pushed
void back(){
	if(initialScreen == 1){
		//POP POP! top window
		window_stack_pop(true);
		APP_LOG(APP_LOG_LEVEL_INFO, "Timer+: popping top window...");
	}
	else if(setHour == 1){
		setHour = 0;
		transition(6, 1);
		initialScreen = 1;
		updateActionBar(1);
	}
	else if(setMinute == 1){
		setMinute = 0;
		setHour = 1;
		if(intervalHour == 1){
			desText = 9;
		}
		else{
			desText = 6;
		}
		transition(4, 1);
	}
	else if(setSecond == 1){
		updateActionBar(2);
		setSecond = 0;
		setMinute = 1;
		if(intervalMin != 1){
			desText = 7;
		}
		else{
			desText = 5;
		}
		transition(1, 1);
	}
	else if(running_stopwatch == 1 || running_stopwatch == 2){
		initialScreen = 1;
		running_stopwatch = 0;
		sw_seconds = 0;
		sw_minutes = 0;
		sw_hours = 0;
		desText = 6;
		transition(9, 1);
		transition(7, 1);
		updateActionBar(1);
		actionBarAnimate(2);
		start_time = 0;
		elapsed_time = 0;
	}
	else if(running == 1 || running == 2 || ended == 1){
		initialScreen = 1;
		updateActionBar(1);
		setHour = 0;
		setMinute = 0;
		setSecond = 0;
		ended = 0;
		running = 0;
		intervalHour = 0;
		intervalMin = 1;
		intervalSec = 0;
		desText = 6;
		updateMin();
		updateSec();
		updateHour();
		transition(3, 1);
		transition(9, 1);
		transition(7, 1);
		actionBarAnimate(2);
	}
}

void timer_fire(int minutes, int seconds, int hours){
	start1 = GRect(300, 100, 144, 168);
	finish1 = GRect(0, 100, 144, 168);
	animate_layer(text_layer_get_layer(des_text_layer), &finish1, &start1, animationSpeed, 200);
	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
	transition(9, 0);
	timerData.sec = seconds;
	timerData.min = minutes;
	timerData.hour = hours;
	desText = 4;
	running = 1;
	stopVibe = 0;
}

void fire_stopwatch(){
	start_time = float_time_ms();
	tick_timer_service_subscribe(SECOND_UNIT, tick_handler_stopwatch);
	running_stopwatch = 1;
	initialScreen = 0;
	updateActionBar(4);
	transition(8, 0);
	if(settings.hideActionBar != 0){
		actionBarAnimate(1);
	}
}

void on_animation_stopped(Animation *anim, bool finished, void *context)
{
    //Free the memory used by the Animation
    property_animation_destroy((PropertyAnimation*) anim);
	updateDesText(desText);
}
 
void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay)
{
    //Declare animation
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
     
    //Set characteristics
    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);
     
	//And stop handlers
	AnimationHandlers handlers = {
    .stopped = (AnimationStoppedHandler) on_animation_stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);
     
    animation_schedule((Animation*) anim);
}

static TextLayer* textLayerInit(GRect location, GColor colour, GColor background, GTextAlignment alignment, int font)
{
	TextLayer *layer = text_layer_create(location);
	text_layer_set_text_color(layer, colour);
	text_layer_set_background_color(layer, background);
	text_layer_set_text_alignment(layer, alignment);
	if(font == 1){
		text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	}
	else if(font == 2){
		text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	}
	else if(font == 3){
		text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
	}
	return layer;
}


void updateSec(){
	snprintf(timeFormatSec, sizeof(timeFormatSec), "%d", intervalSec);
	text_layer_set_text(sec_text_layer, timeFormatSec);
}

void updateMin(){
	snprintf(timeFormatMin, sizeof(timeFormatMin), "%d", intervalMin);
	text_layer_set_text(min_text_layer, timeFormatMin);
}

void updateHour(){
	snprintf(timeFormatHour, sizeof(timeFormatHour), "%d", intervalHour);
	text_layer_set_text(hour_text_layer, timeFormatHour);
}

void increment_time(ClickRecognizerRef recognizer, void *context){
	stopVibe = 1;
	if(initialScreen == 1){
		transition(6, 0);
		setHour = 1;
		initialScreen = 0;
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, actionIconPlus);
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_DOWN, actionIconMinus);
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_SELECT, actionIconNext);
	}
	else if(setHour == 1){
		if(intervalHour < 23){
			intervalHour++;
			updateHour();
		}
		else{
			intervalHour = 0;
			updateHour();
		}
		if(intervalHour != 1){
			text_layer_set_text(des_text_layer, "Hours");
		}
		else{
			text_layer_set_text(des_text_layer, "Hour");
		}
	}
	else if(setMinute == 1){
		if(intervalMin < 59){
			intervalMin++;
			updateMin();
		}
		else{
			intervalMin = 0;
			updateMin();
		}
		if(intervalMin > 1 || intervalMin < 1){
			text_layer_set_text(des_text_layer, "Minutes");
		}
		else if(intervalMin == 1){
			text_layer_set_text(des_text_layer, "Minute");
		}
	}
	else if(setSecond == 1){
		if(intervalSec < 59){
			intervalSec++;
			updateSec();
		}
		else{
			intervalSec = 0;
			updateSec();
		}
		if(intervalSec > 1 || intervalSec < 1){
			text_layer_set_text(des_text_layer, "Seconds");
		}
		else if(intervalSec == 1){
			text_layer_set_text(des_text_layer, "Second");
		}
	}
	else if(running == 2){
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, actionIconPause);
		actionBarAnimate(2);
		running = 1;
	}
	else if(running == 1){
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, actionIconResume);
		actionBarAnimate(2);
		running = 2;
	}
	else if(ended == 1){
		setHour = 1;
		setMinute = 0;
		setSecond = 0;
		ended = 0;
		running = 0;
		intervalHour = 0;
		intervalMin = 1;
		intervalSec = 0;
		desText = 6;
		updateMin();
		updateSec();
		updateHour();
		transition(5, 0);
		transition(9, 1);
		updateActionBar(2);
	}
	else if(running_stopwatch  == 1){
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, actionIconResume);
		actionBarAnimate(2);
		running_stopwatch = 2;
	}
	else if(running_stopwatch  == 2){
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, actionIconPause);
		if(settings.hideActionBar != 0){
			actionBarAnimate(1);
		}
		running_stopwatch = 1;
	}
}

void decrement_time(ClickRecognizerRef recognizer, void *context){
	stopVibe = 1;
	if(initialScreen == 1){
		APP_LOG(APP_LOG_LEVEL_INFO, "Starting stopwatch");
		transition(7, 0);
		fire_stopwatch();
	}
	else if(setHour == 1){
		if(intervalHour > 0){
			intervalHour--;
			updateHour();
		}
		else{
			intervalHour = 23;
			updateHour();
		}
		if(intervalHour != 1){
			text_layer_set_text(des_text_layer, "Hours");
		}
		else{
			text_layer_set_text(des_text_layer, "Hour");
		}
	}
	else if(setMinute == 1){
		if(intervalMin > 0){
			intervalMin--;
			updateMin();
		}
		else{
			intervalMin = 59;
			updateMin();
		}
		if(intervalMin > 1 || intervalMin < 1){
			text_layer_set_text(des_text_layer, "Minutes");
		}
		else if(intervalMin == 1){
			text_layer_set_text(des_text_layer, "Minute");
		}
	}
	else if(setSecond == 1){
		if(intervalSec > 0){
			intervalSec--;
			updateSec();
		}
		else{
			intervalSec = 59;
			updateSec();
		}
		if(intervalSec > 1 || intervalSec < 1){
			text_layer_set_text(des_text_layer, "Seconds");
		}
		else if(intervalSec == 1){
			text_layer_set_text(des_text_layer, "Second");
		}
	}
	else if(running == 1 || running == 2){
		initialScreen = 1;
		updateActionBar(1);
		setHour = 0;
		setMinute = 0;
		setSecond = 0;
		ended = 0;
		running = 0;
		intervalHour = 0;
		intervalMin = 1;
		intervalSec = 0;
		desText = 6;
		updateMin();
		updateSec();
		updateHour();
		transition(9, 1);
		transition(7, 1);
		actionBarAnimate(2);
	}
	else if(ended == 1){
		ended = 0;
		running = 0;
		desText = 2;
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, actionIconPause);
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_DOWN, actionIconStop);
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_SELECT, actionIconRestart);
		timer_fire(intervalMin, intervalSec, intervalHour);
	}
	else if(running_stopwatch == 1 || running_stopwatch == 2){
		int lapSec = (int)elapsed_time % 60;
    	int lapMin = (int)elapsed_time / 60 % 60;
    	int lapHour = (int)elapsed_time / 3600;
		APP_LOG(APP_LOG_LEVEL_INFO, "Lap set: %d lap min(s), %d lap sec(s), %d lap hour(s)", lapMin, lapSec, lapHour);
		start_time = float_time_ms();
		snprintf(lapBuffer, sizeof(lapBuffer), "Lap: ~%d:%d:%d", lapHour, lapMin, lapSec);
		text_layer_set_text(lap_text_layer, lapBuffer);
		actionBarAnimate(2);
		transition(10, 0);
	}
}

void next_step(ClickRecognizerRef recognizer, void *context){
	stopVibe = 1;
	if(initialScreen == 1){
		window_set_window_handlers(menu_window, (WindowHandlers) {
    	.load = window_load_menu,
    	.unload = window_unload_menu,
    });
	window_stack_push(menu_window, true);
	}
	else if(setHour == 1){
		setHour = 0;
		setMinute = 1;
		desText = 5;
		transition(4, 0);
	}
	else if(setMinute == 1){
		setMinute = 0;
		setSecond = 1;
		desText = 1;
		transition(1, 0);
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_SELECT, actionIconCheckmark);
	}
	else if(setSecond == 1){
		setSecond = 0;
		desText = 2;
		transition(2, 0);
		timer_fire(intervalMin, intervalSec, intervalHour);
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, actionIconPause);
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_DOWN, actionIconStop);
		action_bar_layer_set_icon(main_action_bar, BUTTON_ID_SELECT, actionIconRestart);
		if(settings.hideActionBar != 0){
			actionBarAnimate(1);
		}
	}
	else if(running == 1 || running == 2){
		timer_fire(intervalMin, intervalSec, intervalHour);
		actionBarAnimate(2);
		desText = 2;
	}
	else if(running_stopwatch == 2 || running_stopwatch == 1){
		sw_seconds = 0;
		sw_minutes = 0;
		sw_hours = 0;
		start_time = float_time_ms();
		elapsed_time = 0;
		text_layer_set_text(stopwatch_text_layer1, "0");
		text_layer_set_text(stopwatch_text_layer2, "0");
		text_layer_set_text(stopwatch_text_layer3, "0");
		actionBarAnimate(2);
	}
}

void click_config_provider(void *context) {
  	int interval = 50;
  	window_single_repeating_click_subscribe(BUTTON_ID_UP, interval, (ClickHandler) increment_time);
  	window_single_repeating_click_subscribe(BUTTON_ID_DOWN, interval, (ClickHandler) decrement_time);
  	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) next_step);
	window_single_click_subscribe(BUTTON_ID_BACK, (ClickHandler) back);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed){
		if(running == 1){
			timerData.sec--;
			if(timerData.sec == 0 && timerData.min == 0 && timerData.hour == 0) 
			{
				timeIsUp();
				running = 0;
				text_layer_set_text(stopwatch_text_layer1, "0");
				action_bar_layer_set_icon(main_action_bar, BUTTON_ID_UP, actionIconNext);
				action_bar_layer_set_icon(main_action_bar, BUTTON_ID_DOWN, actionIconRestart);
				action_bar_layer_clear_icon(main_action_bar, BUTTON_ID_SELECT);
				transition(3, 0);
				desText = 4;
				ended = 1;
				return;
			}
			if(timerData.sec < 0)
			{
				if(timerData.min > 0){
					timerData.min--;
					timerData.sec = 59;
				}
				else if(timerData.min == 0 && timerData.hour > 0){
					timerData.sec = 59;
					timerData.min = 59;
					timerData.hour--;
					
				}
			}
			if(actionBarHidden == 0 && timerData.sec % 3 == 0 && settings.hideActionBar != 0){
				actionBarAnimate(1);
			}
			snprintf(timerSec, sizeof(timerSec), "%d", timerData.sec);
			text_layer_set_text(stopwatch_text_layer1, timerSec);
			snprintf(timerMin, sizeof(timerMin), "%d", timerData.min);
			text_layer_set_text(stopwatch_text_layer2, timerMin);
			snprintf(timerHour, sizeof(timerHour), "%d", timerData.hour);
			text_layer_set_text(stopwatch_text_layer3, timerHour);
			if(timerData.sec  == 1){
				text_layer_set_text(seconds_text_layer, "sec");
			}
			else{
				text_layer_set_text(seconds_text_layer, "secs");
			}
			if(timerData.min == 1){
				text_layer_set_text(minutes_text_layer, "min");
			}
			else{
				text_layer_set_text(minutes_text_layer, "mins");
			}
			if(timerData.hour == 1){
				text_layer_set_text(hours_text_layer, "hour");
			}
			else{
				text_layer_set_text(hours_text_layer, "hours");
			}
			if(settings.hideUnused == 1 || settings.hideUnused == 3){
			if(timerData.hour < 1){
				layer_set_hidden(text_layer_get_layer(hours_text_layer), true);
				layer_set_hidden(text_layer_get_layer(stopwatch_text_layer3), true);
			}
			else{
				layer_set_hidden(text_layer_get_layer(hours_text_layer), false);
				layer_set_hidden(text_layer_get_layer(stopwatch_text_layer3), false);
			}
			if(timerData.min < 1 && timerData.hour < 1){
				layer_set_hidden(text_layer_get_layer(minutes_text_layer), true);
				layer_set_hidden(text_layer_get_layer(stopwatch_text_layer2), true);
			}
			else{
				layer_set_hidden(text_layer_get_layer(minutes_text_layer), false);
				layer_set_hidden(text_layer_get_layer(stopwatch_text_layer2), false);
			}
		}
		}
}

void tick_handler_stopwatch(struct tm *tick_time, TimeUnits units_changed){
	if(running_stopwatch == 1){
		now = float_time_ms();
		elapsed_time = now - start_time;
		sw_seconds++;
		if(sw_seconds == 60){
			sw_minutes++;
			sw_seconds = 0;
		}
		if(sw_minutes == 60){
			sw_hours++;
			sw_minutes = 0;
			sw_seconds = 0;
		}
		snprintf(stopwatchSec, sizeof(stopwatchSec), "%d", sw_seconds);
		text_layer_set_text(stopwatch_text_layer1, stopwatchSec);
		snprintf(stopwatchMin, sizeof(stopwatchMin), "%d", sw_minutes);
		text_layer_set_text(stopwatch_text_layer2, stopwatchMin);
		snprintf(stopwatchHour, sizeof(stopwatchHour), "%d", sw_hours);
		text_layer_set_text(stopwatch_text_layer3, stopwatchHour);
		if(sw_minutes % 1 == 0 && sw_seconds == 0 && settings.stopwatchTimer == 1){
			vibes_short_pulse();
		}
		else if(sw_minutes % 2 == 0 && sw_seconds == 0 && settings.stopwatchTimer == 2){
			vibes_short_pulse();
		}
		else if(sw_minutes % 5 == 0 && sw_seconds == 0 && settings.stopwatchTimer == 3){
			vibes_short_pulse();
		}
		else if(sw_minutes % 10 == 0 && sw_seconds == 0 && settings.stopwatchTimer == 4){
			vibes_short_pulse();
		}
		else if(sw_minutes % 15 == 0 && sw_seconds == 0 && settings.stopwatchTimer == 5){
			vibes_short_pulse();
		}
		else if(sw_minutes % 30 == 0 && sw_seconds == 0 && settings.stopwatchTimer == 6){
			vibes_short_pulse();
		}
		else if(sw_hours % 1 == 0 && sw_seconds == 0 && settings.stopwatchTimer == 7){
			vibes_short_pulse();
		}
		if(actionBarHidden == 0 && sw_seconds % 3 == 0 && settings.hideActionBar != 0){
				actionBarAnimate(1);
			}
		if(sw_seconds  == 1){
				text_layer_set_text(seconds_text_layer, "sec");
		}
		else{
			text_layer_set_text(seconds_text_layer, "secs");
		}
		if(sw_minutes == 1){
			text_layer_set_text(minutes_text_layer, "min");
		}
		else{
			text_layer_set_text(minutes_text_layer, "mins");
		}
		if(sw_hours == 1){
			text_layer_set_text(hours_text_layer, "hour");
		}
		else{
			text_layer_set_text(hours_text_layer, "hours");
		}
		if(settings.hideUnused == 1 || settings.hideUnused == 2){
			if(sw_hours < 1){
				layer_set_hidden(text_layer_get_layer(hours_text_layer), true);
				layer_set_hidden(text_layer_get_layer(stopwatch_text_layer3), true);
			}
			else{
				layer_set_hidden(text_layer_get_layer(hours_text_layer), false);
				layer_set_hidden(text_layer_get_layer(stopwatch_text_layer3), false);
			}
			if(sw_minutes < 1 && sw_hours < 1){
				layer_set_hidden(text_layer_get_layer(minutes_text_layer), true);
				layer_set_hidden(text_layer_get_layer(stopwatch_text_layer2), true);
			}
			else{
				layer_set_hidden(text_layer_get_layer(minutes_text_layer), false);
				layer_set_hidden(text_layer_get_layer(stopwatch_text_layer2), false);
			}
		}
		
	}
}

static void theme_callback(int index, void *ctx) {
	settings.theme = !settings.theme;
	
	if(settings.theme == 1){
		first_menu_items[0].subtitle = "White on black";
	}
	else{
		first_menu_items[0].subtitle = "Black on white";
	}
  layer_mark_dirty(simple_menu_layer_get_layer(options));
}

static void timerVibrate_callback(int index, void *ctx){
	settings.vibrate++;
	
	if(settings.vibrate == 1){
		second_menu_items[0].subtitle = "Short pulse";
	}
	else if(settings.vibrate == 2){
		second_menu_items[0].subtitle = "Double pulse";
	}
	else if(settings.vibrate == 3){
		second_menu_items[0].subtitle = "Long pulse";
	}
	else if(settings.vibrate > 3 || settings.vibrate == 0){
		settings.vibrate = 0;
		second_menu_items[0].subtitle = "Off";
	}
	layer_mark_dirty(simple_menu_layer_get_layer(options));
}

static void stopwatchVibrate_callback(int index, void *ctx){
	settings.stopwatchTimer++;
	if(settings.stopwatchTimer == 0){
		third_menu_items[0].subtitle = "Off";
	}
	else if(settings.stopwatchTimer == 1){
		third_menu_items[0].subtitle = "minute";
	}
	else if(settings.stopwatchTimer == 2){
		third_menu_items[0].subtitle = "2 minutes";
	}
	else if(settings.stopwatchTimer == 3){
		third_menu_items[0].subtitle = "5 minutes";
	}
	else if(settings.stopwatchTimer == 4){
		third_menu_items[0].subtitle = "10 minutes";
	}
	else if(settings.stopwatchTimer == 5){
		third_menu_items[0].subtitle = "15 minutes";
	}
	else if(settings.stopwatchTimer == 6){
		third_menu_items[0].subtitle = "30 minutes";
	}
	else if(settings.stopwatchTimer == 7){
		third_menu_items[0].subtitle = "hour";
	}
	else if(settings.stopwatchTimer > 7){
		settings.stopwatchTimer = 0;
		third_menu_items[0].subtitle = "Off";
	}
	layer_mark_dirty(simple_menu_layer_get_layer(options));
}

static void actionbar_callback(int index, void *ctx){
	settings.hideActionBar++;
	
	if(settings.hideActionBar == 1){
		first_menu_items[1].subtitle = "While active";
	}
	else if(settings.hideActionBar == 2){
		first_menu_items[1].subtitle = "Always hide";
	}
	else if(settings.hideActionBar > 2 || settings.hideActionBar == 0){
		settings.hideActionBar = 0;
		first_menu_items[1].subtitle = "Never hide";
	}
  layer_mark_dirty(simple_menu_layer_get_layer(options));
}

static void statusbar_callback(int index, void *ctx){
	settings.hideStatusBar = !settings.hideStatusBar;
	if(settings.hideStatusBar == 1){
		first_menu_items[2].subtitle = "On (warning: buggy!)";
	}
	else{
		first_menu_items[2].subtitle = "Off";
	}
	layer_mark_dirty(simple_menu_layer_get_layer(options));
}

static void thanks(int index, void *ctx){
	fourth_menu_items[1].subtitle = "Thanks pedrolane :)";
	layer_mark_dirty(simple_menu_layer_get_layer(options));
}

static void version_callback(int index, void *ctx){
	fourth_menu_items[2].subtitle = "Updated June 12th, '14";
	layer_mark_dirty(simple_menu_layer_get_layer(options));
}

static void ilovecommunity(int index, void *ctx){
	fourth_menu_items[0].title = "#Six seasons";
	fourth_menu_items[0].subtitle = "and a movie :D";
	layer_mark_dirty(simple_menu_layer_get_layer(options));
}

void animationspeed_callback(int index, void *ctx){
	settings.animationSpeed++;
	if(settings.animationSpeed == 0){
		first_menu_items[4].subtitle = "Quick (300ms)";
		animationSpeed = 300;
	}
	else if(settings.animationSpeed == 1){
		first_menu_items[4].subtitle = "Medium (600ms)";
		animationSpeed = 600;
	}
	else if(settings.animationSpeed == 2){
		first_menu_items[4].subtitle = "Slow (900ms)";
		animationSpeed = 900;
	}
	else if(settings.animationSpeed == 3){
		settings.animationSpeed = 0;
		first_menu_items[4].subtitle = "Quick (300ms)";
		animationSpeed = 300;
	}
	layer_mark_dirty(simple_menu_layer_get_layer(options));
}

static void hideunused_callback(int index, void *ctx){
	settings.hideUnused++;
	
	if(settings.hideUnused == 0){
		first_menu_items[3].subtitle = "Off (won't hide)";
	}
	else if(settings.hideUnused == 1){
		first_menu_items[3].subtitle = "On (will hide)";
	}
	else if(settings.hideUnused == 2){
		first_menu_items[3].subtitle = "Only for stopwatch";
	}
	else if(settings.hideUnused == 3){
		first_menu_items[3].subtitle = "Only for timer";
	}
	else if(settings.hideUnused > 3){
		settings.hideUnused = 0;
		first_menu_items[3].subtitle = "Off (won't hide)";
	}
	layer_mark_dirty(simple_menu_layer_get_layer(options));
}

void window_load_menu(Window *window){
  int num_a_items = 0;

  	first_menu_items[0] = (SimpleMenuItem){
    	.title = "Theme",
		//I GOT A CALLBACK! 
		// https://www.youtube.com/watch?v=GgGy_ToeYtc
    	.callback = theme_callback,
    };
	first_menu_items[1] = (SimpleMenuItem){
    	.title = "Hide ActionBar",
    	.callback = actionbar_callback,
    };
	first_menu_items[2] = (SimpleMenuItem){
		.title = "Hide Status Bar",
		.callback = statusbar_callback,
	};
	first_menu_items[3] = (SimpleMenuItem){
		.title = "Hide Unused #s",
		.callback = hideunused_callback,
	};
	first_menu_items[4] = (SimpleMenuItem){
		.title = "Animation Speed",
		.callback = animationspeed_callback,
	};

  	second_menu_items[0] = (SimpleMenuItem){
    	.title = "Vibrate",
		.subtitle = "On",
    	.callback = timerVibrate_callback,
    };
	
	third_menu_items[0] = (SimpleMenuItem){
		.title = "Vibrate every...",
		.subtitle = "Off",
		.callback = stopwatchVibrate_callback,
	};
	
	fourth_menu_items[0] = (SimpleMenuItem){
		.title = "By: Edwin Finch",
		.subtitle = "edwinfinch.com",
		.callback = ilovecommunity,
	};
	fourth_menu_items[1] = (SimpleMenuItem){
		.title = "Anim. ActionBar",
		.subtitle = "bit.ly/animaction1",
		.callback = thanks,
	};
	fourth_menu_items[2] = (SimpleMenuItem){
		.title = "Timer+ Version",
		.subtitle = "0.7.2 Stable Build 2a",
		.callback = version_callback,
	};

  menu_sections[0] = (SimpleMenuSection){
	.title = "General",
  	.num_items = NUM_FIRST_MENU_ITEMS,
    .items = first_menu_items,
  };
  menu_sections[1] = (SimpleMenuSection){
    .title = "Timer",
    .num_items = NUM_SECOND_MENU_ITEMS,
    .items = second_menu_items,
  };
	menu_sections[2] = (SimpleMenuSection){
    .title = "Stopwatch",
    .num_items = NUM_THIRD_MENU_ITEMS,
    .items = third_menu_items,
  };
	menu_sections[3] = (SimpleMenuSection){
    .title = "Aboot",
    .num_items = NUM_FOURTH_MENU_ITEMS,
    .items = fourth_menu_items,
  };

	if(settings.theme == 1){
		first_menu_items[0].subtitle = "White on black";
	}
	else{
		first_menu_items[0].subtitle = "Black on white";
	}
	
	if(settings.hideActionBar == 1){
		first_menu_items[1].subtitle = "While active";
	}
	else if(settings.hideActionBar == 2){
		first_menu_items[1].subtitle = "Always hide";
	}
	else if(settings.hideActionBar > 2 || settings.hideActionBar == 0){
		first_menu_items[1].subtitle = "Never hide";
	}
	
	if(settings.hideStatusBar == 1){
		first_menu_items[2].subtitle = "On (warning: buggy!)";
	}
	else{
		first_menu_items[2].subtitle = "Off";
	}

	if(settings.hideUnused == 0){
		first_menu_items[3].subtitle = "Off (won't hide)";
	}
	else if(settings.hideUnused == 1){
		first_menu_items[3].subtitle = "On (will hide)";
	}
	else if(settings.hideUnused == 2){
		first_menu_items[3].subtitle = "Only for stopwatch";
	}
	else if(settings.hideUnused == 3){
		first_menu_items[3].subtitle = "Only for timer";
	}
	else if(settings.hideUnused > 3){
		settings.hideUnused = 0;
		first_menu_items[3].subtitle = "Off (won't hide)";
	}
	
	if(settings.vibrate == 1){
		second_menu_items[0].subtitle = "Short pulse";
	}
	else if(settings.vibrate == 2){
		second_menu_items[0].subtitle = "Double pulse";
	}
	else if(settings.vibrate == 3){
		second_menu_items[0].subtitle = "Long pulse";
	}
	else if(settings.vibrate > 3 || settings.vibrate == 0){
		settings.vibrate = 0;
		second_menu_items[0].subtitle = "Off";
	}
	
	if(settings.stopwatchTimer == 0){
		third_menu_items[0].subtitle = "Off";
	}
	else if(settings.stopwatchTimer == 1){
		third_menu_items[0].subtitle = "minute";
	}
	else if(settings.stopwatchTimer == 2){
		third_menu_items[0].subtitle = "2 minutes";
	}
	else if(settings.stopwatchTimer == 3){
		third_menu_items[0].subtitle = "5 minutes";
	}
	else if(settings.stopwatchTimer == 4){
		third_menu_items[0].subtitle = "10 minutes";
	}
	else if(settings.stopwatchTimer == 5){
		third_menu_items[0].subtitle = "15 minutes";
	}
	else if(settings.stopwatchTimer == 6){
		third_menu_items[0].subtitle = "30 minutes";
	}
	else if(settings.stopwatchTimer == 7){
		third_menu_items[0].subtitle = "hour";
	}
	else if(settings.stopwatchTimer > 7){
		third_menu_items[0].subtitle = "Off";
	}
	if(settings.animationSpeed == 0){
		first_menu_items[4].subtitle = "Quick (300ms)";
		animationSpeed = 300;
	}
	else if(settings.animationSpeed == 1){
		first_menu_items[4].subtitle = "Medium (600ms)";
		animationSpeed = 600;
	}
	else if(settings.animationSpeed == 2){
		first_menu_items[4].subtitle = "Slow (900ms)";
		animationSpeed = 900;
	}
	else{
		settings.animationSpeed = 0;
		animationSpeed = 300;
	}
	
  	Layer *window_layer = window_get_root_layer(window);
  	GRect bounds = layer_get_frame(window_layer);

  	options = simple_menu_layer_create(bounds, window, menu_sections, NUM_MENU_SECTIONS, NULL);
  	layer_add_child(window_layer, simple_menu_layer_get_layer(options));
}

void window_unload_menu(Window *window){
	simple_menu_layer_destroy(options);
	if(settings.theme == 1){
		layer_set_hidden(inverter_layer_get_layer(theme), false);
	}
	else if(settings.theme == 0){
		layer_set_hidden(inverter_layer_get_layer(theme), true);
	}
	
	if(settings.hideActionBar == 1 || settings.hideActionBar == 0 || settings.hideActionBar == 3){
		updateActionBar(1);
		layer_set_hidden(action_bar_layer_get_layer(main_action_bar), false);
	}
	else if(settings.hideActionBar == 2){
		layer_set_hidden(action_bar_layer_get_layer(main_action_bar), true);
	}
	
	if(settings.hideStatusBar == 1){
		window_set_fullscreen(window, true);
	}
	else{
		window_set_fullscreen(window, false);
	}
}

void window_load(Window *window){
	Layer *window_layer = window_get_root_layer(window);
	actionIconPlus = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PLUS);
	actionIconMinus = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MINUS);
	actionIconNext = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NEXT);
	actionIconCheckmark = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHECKMARK);
	actionIconPause = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PAUSE);
	actionIconResume = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RESUME);
	actionIconRestart = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RESTART);
	actionIconStop = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STOP);
	actionIconSettings = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SETTINGS);
	actionIconTimer = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TIMER);
	actionIconStopwatch = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STOPWATCH);
	actionIconLap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LAP);
	min_text_layer = textLayerInit(GRect(160, 0, 144, 168), GColorBlack, GColorClear, GTextAlignmentCenter, 1);
	sec_text_layer = textLayerInit(GRect(160, 0, 144, 168), GColorBlack, GColorClear, GTextAlignmentCenter, 1);
	hour_text_layer = textLayerInit(GRect(160, 0, 144, 168), GColorBlack, GColorClear, GTextAlignmentCenter, 1);
	des_text_layer = textLayerInit(GRect(160, 100, 144, 168), GColorBlack, GColorWhite, GTextAlignmentCenter, 2);
	option_text_layer1 = textLayerInit(GRect(160, -100, 144, 168), GColorBlack, GColorClear, GTextAlignmentCenter, 3);
	option_text_layer2 = textLayerInit(GRect(160, 0, 144, 168), GColorBlack, GColorClear, GTextAlignmentCenter, 3);
	option_text_layer3 = textLayerInit(GRect(160, 100, 144, 168), GColorBlack, GColorClear, GTextAlignmentCenter, 3);
	timer_text_layer = textLayerInit(GRect(160, 100, 144, 168), GColorBlack, GColorClear, GTextAlignmentCenter, 1);
	stopwatch_text_layer1 = textLayerInit(GRect(160, 100, 144, 168), GColorBlack, GColorClear, GTextAlignmentCenter, 1);
	stopwatch_text_layer2 = textLayerInit(GRect(160, 100, 144, 168), GColorBlack, GColorClear, GTextAlignmentCenter, 1);
	stopwatch_text_layer3 = textLayerInit(GRect(160, 100, 144, 168), GColorBlack, GColorClear, GTextAlignmentCenter, 1);
	minutes_text_layer = textLayerInit(GRect(160, 100, 144, 168), GColorBlack, GColorClear, GTextAlignmentCenter, 3);
	seconds_text_layer = textLayerInit(GRect(160, 100, 144, 168), GColorBlack, GColorClear, GTextAlignmentCenter, 3);
	hours_text_layer = textLayerInit(GRect(160, 100, 144, 168), GColorBlack, GColorClear, GTextAlignmentCenter, 3);
	lap_text_layer = textLayerInit(GRect(160, 100, 144, 50), GColorBlack, GColorWhite, GTextAlignmentCenter, 3);
	intervalMin = 1;
	intervalSec = 0;
	intervalHour = 0;
	snprintf(timeFormatMin, sizeof(timeFormatMin), "%d", intervalMin);
	text_layer_set_text(min_text_layer, timeFormatMin);
	snprintf(timeFormatSec, sizeof(timeFormatSec), "%d", intervalSec);
	text_layer_set_text(sec_text_layer, timeFormatSec);
	snprintf(timeFormatHour, sizeof(timeFormatHour), "%d", intervalHour);
	text_layer_set_text(hour_text_layer, timeFormatHour);
	text_layer_set_text(des_text_layer, "Hours");
	text_layer_set_text(option_text_layer1, "Timer");
	text_layer_set_text(option_text_layer2, "Options");
	text_layer_set_text(option_text_layer3, "Stopwatch");
	text_layer_set_text(stopwatch_text_layer1, "0");
	text_layer_set_text(stopwatch_text_layer2, "0");
	text_layer_set_text(stopwatch_text_layer3, "0");
	text_layer_set_text(minutes_text_layer, "min");
	text_layer_set_text(seconds_text_layer, "sec");
	text_layer_set_text(hours_text_layer, "hour");
	layer_add_child(window_layer, (Layer*) min_text_layer);
	layer_add_child(window_layer, (Layer*) sec_text_layer);
	layer_add_child(window_layer, (Layer*) hour_text_layer);
	layer_add_child(window_layer, (Layer*) timer_text_layer);
	layer_add_child(window_layer, (Layer*) option_text_layer1);
	layer_add_child(window_layer, (Layer*) option_text_layer2);
	layer_add_child(window_layer, (Layer*) option_text_layer3);
	layer_add_child(window_layer, (Layer*) stopwatch_text_layer1);
	layer_add_child(window_layer, (Layer*) stopwatch_text_layer2);
	layer_add_child(window_layer, (Layer*) stopwatch_text_layer3);
	layer_add_child(window_layer, (Layer*) minutes_text_layer);
	layer_add_child(window_layer, (Layer*) seconds_text_layer);
	layer_add_child(window_layer, (Layer*) hours_text_layer);
	layer_add_child(window_layer, (Layer*) lap_text_layer);
	layer_add_child(window_layer, (Layer*) des_text_layer);
	main_action_bar = action_bar_layer_create();
	action_bar_layer_add_to_window(main_action_bar, window);
	action_bar_layer_set_click_config_provider(main_action_bar, click_config_provider);
	theme = inverter_layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_layer, (Layer*) theme);
	
	if(settings.theme == 0){
		layer_set_hidden(inverter_layer_get_layer(theme), true);
	}
	else{
		layer_set_hidden(inverter_layer_get_layer(theme), false);
	}
	
	if(settings.hideActionBar != 2){
		updateActionBar(1);
		layer_set_hidden(action_bar_layer_get_layer(main_action_bar), false);
	}
	else if(settings.hideActionBar == 2){
		layer_set_hidden(action_bar_layer_get_layer(main_action_bar), true);
	}
	
	if(settings.hideStatusBar == 1){
		window_set_fullscreen(window, true);
	}
	else{
		window_set_fullscreen(window, false);
	}
	
	start = GRect(0, -50, 144, 168);
	finish = GRect(20, 10, 144, 168);
	finish1 = GRect(13, 55, 144, 168);
	finish2 = GRect(-2, 103, 144, 168);
	animate_layer(text_layer_get_layer(option_text_layer1), &start, &finish, animationSpeed, 100);
	animate_layer(text_layer_get_layer(option_text_layer2), &start, &finish1, animationSpeed, 200);
	animate_layer(text_layer_get_layer(option_text_layer3), &start, &finish2, animationSpeed, 300);
}

void window_unload(Window *window){
	destroy_property_animation(&prop_animation);
	gbitmap_destroy(actionIconPlus);
	gbitmap_destroy(actionIconMinus);
	gbitmap_destroy(actionIconNext);
	gbitmap_destroy(actionIconCheckmark);
	gbitmap_destroy(actionIconPause);
	gbitmap_destroy(actionIconResume);
	gbitmap_destroy(actionIconRestart);
	gbitmap_destroy(actionIconStop);
	gbitmap_destroy(actionIconSettings);
	gbitmap_destroy(actionIconStopwatch);
	gbitmap_destroy(actionIconTimer);
	gbitmap_destroy(actionIconLap);
	text_layer_destroy(min_text_layer);
	text_layer_destroy(sec_text_layer);
	text_layer_destroy(des_text_layer);
	text_layer_destroy(timer_text_layer);
	text_layer_destroy(option_text_layer1);
	text_layer_destroy(option_text_layer2);
	text_layer_destroy(option_text_layer3);
	text_layer_destroy(stopwatch_text_layer1);
	text_layer_destroy(stopwatch_text_layer2);
	text_layer_destroy(stopwatch_text_layer3);
	text_layer_destroy(minutes_text_layer);
	text_layer_destroy(seconds_text_layer);
	text_layer_destroy(hours_text_layer);
	text_layer_destroy(lap_text_layer);
	if(inverter_layer_get_layer(theme)){
		inverter_layer_destroy(theme);
	}
	action_bar_layer_destroy(main_action_bar);
	tick_timer_service_unsubscribe();
}

void handle_init(void) {
  	window = window_create();
	menu_window = window_create();
  	window_set_window_handlers(window, (WindowHandlers) {
    	.load = window_load,
    	.unload = window_unload,
    });
	valueRead = persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
	if(persist_exists(SETTINGS_KEY)){
		APP_LOG(APP_LOG_LEVEL_INFO, "%d bytes read from settings.", valueRead);
	}
	else{
		APP_LOG(APP_LOG_LEVEL_INFO, "No values present, loading defaults.");
		settings.vibrate = 1;
		settings.theme = 0;
		settings.hideActionBar = 1;
		settings.hideStatusBar = 0;
		settings.hideUnused = 0;
		settings.stopwatchTimer = 0;
	}
	if(settings.animationSpeed == 0){
		first_menu_items[4].subtitle = "Quick (300ms)";
		animationSpeed = 300;
	}
	else if(settings.animationSpeed == 1){
		first_menu_items[4].subtitle = "Medium (600ms)";
		animationSpeed = 600;
	}
	else if(settings.animationSpeed == 2){
		first_menu_items[4].subtitle = "Slow (900ms)";
		animationSpeed = 900;
	}
	else{
		settings.animationSpeed = 1;
		first_menu_items[4].subtitle = "Medium (600ms)";
		animationSpeed = 600;
	}
	window_stack_push(window, true);
}

void handle_deinit(void) {
	valueWritten = persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
	APP_LOG(APP_LOG_LEVEL_INFO, "%d bytes written to settings.", valueWritten);
  	window_destroy(window);
	window_destroy(menu_window);
}

int main(void) {
  	handle_init();
  	app_event_loop();
  	handle_deinit();
}
