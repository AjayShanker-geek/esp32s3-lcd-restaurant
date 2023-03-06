/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "bsp/esp-bsp.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lv_demos.h"

static char *TAG = "app_main";

#define LOG_MEM_INFO (0)

void app_main(void) {
  bsp_i2c_init();
  lv_disp_t *disp = bsp_display_start();

  // ESP_LOGI(TAG, "Display LVGL demo");
  bsp_display_lock(0);
  lv_demo_widgets(); /* A widgets example */

  bsp_display_unlock();

#if LOG_MEM_INFO
  static char buffer[128]; /* Make sure buffer is enough for `sprintf` */
  while (1) {
    sprintf(buffer,
            "   Biggest /     Free /    Total\n"
            "\t  SRAM : [%8d / %8d / %8d]\n"
            "\t PSRAM : [%8d / %8d / %8d]",
            heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL),
            heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
            heap_caps_get_total_size(MALLOC_CAP_INTERNAL),
            heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM),
            heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
            heap_caps_get_total_size(MALLOC_CAP_SPIRAM));
    ESP_LOGI("MEM", "%s", buffer);

    // sprintf("Ajay");

    vTaskDelay(pdMS_TO_TICKS(500));
  }
#endif
}
