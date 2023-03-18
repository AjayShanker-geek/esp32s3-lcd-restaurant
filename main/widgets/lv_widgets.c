/**
 * @file lv_widgets.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_widgets.h"

#if LV_USE_DEMO_WIDGETS

#if LV_MEM_CUSTOM == 0 && LV_MEM_SIZE < (38ul * 1024ul)
#error Insufficient memory for lv_demo_widgets. Please set LV_MEM_SIZE to at least 38KB (38ul * 1024ul).  48KB is recommended.
#endif

#include "mqtt_client.h"
void mqtt_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data);
extern esp_mqtt_event_handle_t event;
extern esp_mqtt_client_handle_t client;
/*********************
 *      DEFINES
 *********************/
#define SEND_BUTTON 7
#define CANCLE_BUTTON 6
#define TABLE_1_BUTTON 0
#define TABLE_2_BUTTON 1
#define TABLE_3_BUTTON 2
#define TABLE_4_BUTTON 3
#define TABLE_5_BUTTON 4
#define TABLE_6_BUTTON 5

uint32_t selected_table = -1;
char* msg = "test";

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
  DISP_SMALL,
  DISP_MEDIUM,
  DISP_LARGE,
} disp_size_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void restaurant_create(lv_obj_t* parent);

/**********************
 *  STATIC VARIABLES
 **********************/
static disp_size_t disp_size;

static lv_obj_t* tv;
static lv_style_t style_text_muted;
static lv_style_t style_title;
static lv_style_t style_icon;
static lv_style_t style_bullet;

static const lv_font_t* font_large;
static const lv_font_t* font_normal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_widgets(void) {
  if (LV_HOR_RES <= 320)
    disp_size = DISP_SMALL;
  else if (LV_HOR_RES < 720)
    disp_size = DISP_MEDIUM;
  else
    disp_size = DISP_LARGE;

  font_large = LV_FONT_DEFAULT;
  font_normal = LV_FONT_DEFAULT;

  lv_coord_t tab_h;
  if (disp_size == DISP_LARGE) {
    tab_h = 70;
#if LV_FONT_MONTSERRAT_24
    font_large = &lv_font_montserrat_24;
#else
    LV_LOG_WARN(
        "LV_FONT_MONTSERRAT_24 is not enabled for the widgets demo. Using "
        "LV_FONT_DEFAULT instead.");
#endif
#if LV_FONT_MONTSERRAT_16
    font_normal = &lv_font_montserrat_16;
#else
    LV_LOG_WARN(
        "LV_FONT_MONTSERRAT_16 is not enabled for the widgets demo. Using "
        "LV_FONT_DEFAULT instead.");
#endif
  } else if (disp_size == DISP_MEDIUM) {
    tab_h = 45;
#if LV_FONT_MONTSERRAT_20
    font_large = &lv_font_montserrat_20;
#else
    LV_LOG_WARN(
        "LV_FONT_MONTSERRAT_20 is not enabled for the widgets demo. Using "
        "LV_FONT_DEFAULT instead.");
#endif
#if LV_FONT_MONTSERRAT_14
    font_normal = &lv_font_montserrat_14;
#else
    LV_LOG_WARN(
        "LV_FONT_MONTSERRAT_14 is not enabled for the widgets demo. Using "
        "LV_FONT_DEFAULT instead.");
#endif
  } else { /* disp_size == DISP_SMALL */
    tab_h = 45;
#if LV_FONT_MONTSERRAT_18
    font_large = &lv_font_montserrat_18;
#else
    LV_LOG_WARN(
        "LV_FONT_MONTSERRAT_18 is not enabled for the widgets demo. Using "
        "LV_FONT_DEFAULT instead.");
#endif
#if LV_FONT_MONTSERRAT_12
    font_normal = &lv_font_montserrat_12;
#else
    LV_LOG_WARN(
        "LV_FONT_MONTSERRAT_12 is not enabled for the widgets demo. Using "
        "LV_FONT_DEFAULT instead.");
#endif
  }

#if LV_USE_THEME_DEFAULT
  lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE),
                        lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK,
                        font_normal);
#endif

  lv_style_init(&style_text_muted);
  lv_style_set_text_opa(&style_text_muted, LV_OPA_50);

  lv_style_init(&style_title);
  lv_style_set_text_font(&style_title, font_large);

  lv_style_init(&style_icon);
  lv_style_set_text_color(&style_icon, lv_theme_get_color_primary(NULL));
  lv_style_set_text_font(&style_icon, font_large);

  lv_style_init(&style_bullet);
  lv_style_set_border_width(&style_bullet, 0);
  lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);

  tv = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, tab_h);

  lv_obj_set_style_text_font(lv_scr_act(), font_normal, 0);

  // lv_obj_t* t1 = lv_tabview_add_tab(tv, "Profile");
  // lv_obj_t* t2 = lv_tabview_add_tab(tv, "Analytics");
  // lv_obj_t* t3 = lv_tabview_add_tab(tv, "Shop");
  // profile_create(t1);
  // analytics_create(t2);
  // shop_create(t3);

  lv_obj_t* t1 = lv_tabview_add_tab(tv, "Restaurant");
  lv_obj_t* t2 = lv_tabview_add_tab(tv, "Settings");
  restaurant_create(t1);

  // color_changer_create(tv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void table1_event_cb(lv_obj_t* obj, lv_event_t event) {
  printf("Clicked\n");
}

static void event_cb(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t* obj = lv_event_get_target(e);
  if (code == LV_EVENT_DRAW_PART_BEGIN) {
    lv_obj_draw_part_dsc_t* dsc = lv_event_get_draw_part_dsc(e);

    /*When the button matrix draws the buttons...*/
    if (dsc->class_p == &lv_btnmatrix_class &&
        dsc->type == LV_BTNMATRIX_DRAW_PART_BTN) {
      /*Change the draw descriptor of the 2nd button*/
      dsc->rect_dsc->radius = 0x06;
      if (dsc->id == SEND_BUTTON) {
        dsc->rect_dsc->bg_color = lv_palette_main(LV_PALETTE_GREEN);
      } else if (dsc->id == CANCLE_BUTTON) {
        dsc->rect_dsc->bg_color = lv_palette_main(LV_PALETTE_RED);
      } else if (lv_btnmatrix_get_selected_btn(obj) == dsc->id &&
                 dsc->id != SEND_BUTTON) {
        dsc->rect_dsc->bg_color = lv_palette_main(LV_PALETTE_LIGHT_GREEN);
        // printf("%i\n", dsc->id);
      } else
        dsc->rect_dsc->bg_color = lv_palette_main(LV_PALETTE_LIGHT_BLUE);

      dsc->label_dsc->color = lv_color_white();
    }
  } else if (code == LV_EVENT_VALUE_CHANGED) {
    uint32_t id = lv_btnmatrix_get_selected_btn(obj);
    const char* txt = lv_btnmatrix_get_btn_text(obj, id);

    if (id == CANCLE_BUTTON)
      selected_table = -1;
    else if (id != SEND_BUTTON) {
      selected_table = id;
      sprintf(msg, "%d", selected_table + 1);
    } else if (selected_table != -1) {
      printf("%i table was sent!\n", selected_table + 1);
      esp_mqtt_client_publish(client, "/topic/table", msg, 0, 0, 0);
      // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
      selected_table = -1;
    }
  }
}

static const char* btnm_map[] = {"Table 1", "Table 2", "Table 3", "\n",
                                 "Table 4", "Table 5", "Table 6", "\n",
                                 "Cancel", "Send", ""};

/**
 * Add custom drawer to the button matrix to customize buttons one by one
 */
void lv_btnmatrix(lv_obj_t* parent) {
  lv_obj_t* btnm = lv_btnmatrix_create(lv_scr_act());
  lv_btnmatrix_set_map(btnm, btnm_map);
  lv_obj_add_event_cb(btnm, event_cb, LV_EVENT_ALL, NULL);
  lv_obj_center(btnm);
  lv_obj_set_size(btnm, 450, 250);
}

static void restaurant_create(lv_obj_t* parent) {
  // lv_obj_t* panel1 = lv_obj_create(parent);
  // lv_obj_set_height(panel1, LV_SIZE_CONTENT);

  // LV_IMG_DECLARE(table_top_view);

  lv_btnmatrix(parent);

  // lv_obj_t* icon;
  // icon = lv_imgbtn_create(panel1);
  // lv_imgbtn_set_src(icon, LV_IMGBTN_STATE_RELEASED, &table_top_view,
  //                   &table_top_view, &table_top_view);
  // lv_obj_add_style(icon, &style_def, 0);
  // lv_obj_add_style(icon, &style_pr, LV_STATE_PRESSED);
  // lv_obj_set_grid_cell(icon, LV_GRID_ALIGN_START, 0, 0,
  // LV_GRID_ALIGN_CENTER, 0,
  //                      1);
  // lv_obj_t* label = lv_label_create(icon);
  // lv_label_set_text(label, "Table 1");
  // lv_obj_add_event_cb(icon, table1_event_cb, LV_EVENT_CLICKED, NULL);
}

#endif
