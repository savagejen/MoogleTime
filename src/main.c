/*

     <sprite>
+-----------------+
|                 |
|   HP   11/23    |
|   MP   30/59    |
+-----------------+

*/
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "resource_ids.auto.h"

#define MY_UUID { 0xAB, 0x51, 0xA1, 0x98, 0xD1, 0xD9, 0x43, 0xB9, 0x9C, 0x06, 0x61, 0xB8, 0xD6, 0x8B, 0x55, 0x5D }
PBL_APP_INFO(MY_UUID,
             "Moogle", "savagejen",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
TextLayer text_hours_layer;
TextLayer text_minutes_layer;
Layer background_layer;

BmpContainer image_container;
int current_background; //0=Awake, 1=Tired, 2=Asleep

void set_background_image(BmpContainer *bmp_container, const int resource_id) {
  //Remove the old image
  layer_remove_from_parent(&bmp_container->layer.layer);
  bmp_deinit_container(bmp_container);
  //Display the new image
  bmp_init_container(resource_id, bmp_container);
  layer_add_child(&background_layer, &bmp_container->layer.layer);
}


void init_text() {
	
  GFont custom_font = \
	  fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FFVI_40));

  //Displays the hours
  text_layer_init(&text_hours_layer, window.layer.frame);
  text_layer_set_text_color(&text_hours_layer, GColorWhite);
  text_layer_set_background_color(&text_hours_layer, GColorClear);
  layer_set_frame(&text_hours_layer.layer, GRect(12, 65, 144-12, 168-65));
  text_layer_set_font(&text_hours_layer, custom_font);
  text_layer_set_text_alignment(&text_hours_layer, GTextAlignmentLeft);
  layer_add_child(&window.layer, &text_hours_layer.layer);

  //Displays the minutes
  text_layer_init(&text_minutes_layer, window.layer.frame);
  text_layer_set_text_color(&text_minutes_layer, GColorWhite);
  text_layer_set_background_color(&text_minutes_layer, GColorClear);
  layer_set_frame(&text_minutes_layer.layer, GRect(12, 100, 144-12, 168-100));
  text_layer_set_font(&text_minutes_layer, custom_font);
  text_layer_set_text_alignment(&text_minutes_layer, GTextAlignmentLeft);
  layer_add_child(&window.layer, &text_minutes_layer.layer);
}

void handle_init(AppContextRef ctx) {
  (void)ctx;
 
  window_init(&window, "Moogle");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);
 
  resource_init_current_app(&APP_RESOURCES);
 
  //Adds Moogle sprite
  layer_init(&background_layer, window.layer.frame);
  layer_add_child(&window.layer, &background_layer);
  current_background=0;
  set_background_image(&image_container,RESOURCE_ID_IMAGE_MOG_OK_WHITE);
 
  //Adds the time text
  init_text();
 
}

void handle_deinit(AppContextRef ctx) {
  (void)ctx;
  bmp_deinit_container(&image_container);
}

//Draws the hours and minutes
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)ctx;

  static char hours_text[] = "    00 00";
  static char minutes_text[] = "    00 00";
	
  char *minutes_format;
  char *hours_format;

  minutes_format = "MP  %M/59";
  if (clock_is_24h_style()) {
	hours_format = "HP  %H/23";
  } else {
	hours_format = "HP  %I/12";
  }

  string_format_time(hours_text, sizeof(hours_text), hours_format, t->tick_time);
  string_format_time(minutes_text, sizeof(minutes_text), minutes_format, t->tick_time);

  if (t->tick_time->tm_hour>0&&t->tick_time->tm_hour<6&&current_background!=2){
	//Moogle is asleep
	set_background_image(&image_container,RESOURCE_ID_IMAGE_MOG_DEAD_WHITE);
	current_background=2;

  }
  if (t->tick_time->tm_hour>6&&t->tick_time->tm_hour<9&&current_background!=1){
	//Moogle is groggy  
	set_background_image(&image_container,RESOURCE_ID_IMAGE_MOG_HURT_WHITE);
	current_background=1;
	
  }
  if (t->tick_time->tm_hour>9&&t->tick_time->tm_hour<21&&current_background!=0){
	//Moogle is awake
	set_background_image(&image_container,RESOURCE_ID_IMAGE_MOG_OK_WHITE);
	current_background=0;

  }
  if (t->tick_time->tm_hour>21&&t->tick_time->tm_hour<24&&current_background!=1){
	//Moogle is tired
	set_background_image(&image_container,RESOURCE_ID_IMAGE_MOG_HURT_WHITE);
	current_background=1;
  }
	
  text_layer_set_text(&text_hours_layer, hours_text);
  text_layer_set_text(&text_minutes_layer, minutes_text);
	
}

void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
	.deinit_handler = &handle_deinit,
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }
  };
  app_event_loop(params, &handlers);
}
