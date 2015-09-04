#include <pebble.h>
  
#define KEY_COLOR_RED     0
#define KEY_COLOR_GREEN   1
#define KEY_COLOR_BLUE    2
#define KEY_COLOR_RED_T     3
#define KEY_COLOR_GREEN_T   4
#define KEY_COLOR_BLUE_T    5
  
static Window *s_main_window;
static TextLayer *s_time_layer;
  
static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  int hour;
  
  // Create a long-lived buffer
  static char buffer[] = "Morning";

  // Let's check the time and display the appropriate text
  hour = tick_time->tm_hour;
  if (hour > 0 && hour <6) {
    strcpy(buffer, "Night");
  }
  else if (hour >= 6 && hour < 12) {
    strcpy(buffer, "Morning");
  }
  else if (hour >= 12 && hour < 15) {
    strcpy(buffer, "Noon");
  }
  else if (hour >= 15 && hour < 21) {
    strcpy(buffer, "Evening");
  }
  else {
    strcpy(buffer, "Night");
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

static void main_window_load(Window *window) {
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 55, 149, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  #ifdef PBL_SDK_2
    // Not available, use normal colors
  #elif PBL_SDK_3
    // Use color setting
    int red = persist_read_int(KEY_COLOR_RED);
    int green = persist_read_int(KEY_COLOR_GREEN);
    int blue = persist_read_int(KEY_COLOR_BLUE);
  
    int red_text = persist_read_int(KEY_COLOR_RED_T);
    int green_text = persist_read_int(KEY_COLOR_GREEN_T);
    int blue_text = persist_read_int(KEY_COLOR_BLUE_T);
    
    // defaults
    if (!persist_exists(KEY_COLOR_RED)) {
      red = 0;
      green = 104;
      blue = 202;
      
      red_text = 255;
      green_text = 255;
      blue_text = 255;
    }

    GColor bg_color = GColorFromRGB(red, green, blue);
    GColor text_color = GColorFromRGB(red_text, green_text, blue_text);
    text_layer_set_text_color(s_time_layer, text_color);   
    window_set_background_color(s_main_window, bg_color);
    text_layer_set_background_color(s_time_layer, bg_color);
       
#endif
  
  // Make sure the time is displayed from the start
  update_time();  
}

static void in_recv_handler(DictionaryIterator *iter, void *context){
  Tuple *color_red_t = dict_find(iter, KEY_COLOR_RED);
  Tuple *color_green_t = dict_find(iter, KEY_COLOR_GREEN);
  Tuple *color_blue_t = dict_find(iter, KEY_COLOR_BLUE);
  
  Tuple *color_red_text_t = dict_find(iter, KEY_COLOR_RED_T);
  Tuple *color_green_text_t = dict_find(iter, KEY_COLOR_GREEN_T);
  Tuple *color_blue_text_t = dict_find(iter, KEY_COLOR_BLUE_T);

    // Apply the color 
#ifdef PBL_SDK_2
    text_layer_set_text_color(s_time_layer, GColorBlack);  
    window_set_background_color(s_main_window, GColorWhite);
  
#elif PBL_SDK_3 
    int red = color_red_t->value->int32;
    int green = color_green_t->value->int32;
    int blue = color_blue_t->value->int32;
  
    int red_text = color_red_text_t->value->int32;
    int green_text = color_green_text_t->value->int32;
    int blue_text = color_blue_text_t->value->int32;

    // Persist values
    persist_write_int(KEY_COLOR_RED, red);
    persist_write_int(KEY_COLOR_GREEN, green);
    persist_write_int(KEY_COLOR_BLUE, blue);
  
    persist_write_int(KEY_COLOR_RED_T, red_text);
    persist_write_int(KEY_COLOR_GREEN_T, green_text);
    persist_write_int(KEY_COLOR_BLUE_T, blue_text);
    
    GColor bg_color = GColorFromRGB(red, green, blue);
    GColor text_color = GColorFromRGB(red_text, green_text, blue_text);
    
    text_layer_set_text_color(s_time_layer, text_color);
    text_layer_set_background_color(s_time_layer, bg_color);
    window_set_background_color(s_main_window, bg_color);
#endif
  
}
  
static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Prepare two-communication for the settings
  app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
