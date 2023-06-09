/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "bsp/esp-bsp.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lv_demos.h"
#include "mqtt_client.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"

static char *TAG = "app_main";

#define CONFIG_BROKER_URL "mqtt://172.20.10.5"
esp_mqtt_event_handle_t event;
esp_mqtt_client_handle_t client;

static void
log_error_if_nonzero(const char *message, int error_code) {
  if (error_code != 0) {
    ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
  }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
  ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
  event = event_data;
  client = event->client;
  int msg_id;
  switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
      ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
      // msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
      // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

      // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
      // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

      // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
      // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

      // msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
      // ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
      break;
    case MQTT_EVENT_DISCONNECTED:
      ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
      break;

    case MQTT_EVENT_SUBSCRIBED:
      ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
      msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
      ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
      break;
    case MQTT_EVENT_UNSUBSCRIBED:
      ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
      break;
    case MQTT_EVENT_PUBLISHED:
      ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
      break;
    case MQTT_EVENT_DATA:
      ESP_LOGI(TAG, "MQTT_EVENT_DATA");
      printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
      printf("DATA=%.*s\r\n", event->data_len, event->data);
      break;
    case MQTT_EVENT_ERROR:
      ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
      if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
        log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
        log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
        log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
        ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
      }
      break;
    default:
      ESP_LOGI(TAG, "Other event id:%d", event->event_id);
      break;
  }
}

static void mqtt_app_start(void) {
  esp_mqtt_client_config_t mqtt_cfg = {
      .broker.address.uri = CONFIG_BROKER_URL,
  };
#if CONFIG_BROKER_URL_FROM_STDIN
  char line[128];

  if (strcmp(mqtt_cfg.broker.address.uri, "FROM_STDIN") == 0) {
    int count = 0;
    printf("Please enter url of mqtt broker\n");
    while (count < 128) {
      int c = fgetc(stdin);
      if (c == '\n') {
        line[count] = '\0';
        break;
      } else if (c > 0 && c < 127) {
        line[count] = c;
        ++count;
      }
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    mqtt_cfg.broker.address.uri = line;
    printf("Broker url: %s\n", line);
  } else {
    ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
    abort();
  }
#endif /* CONFIG_BROKER_URL_FROM_STDIN */

  esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
  /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
  esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
  esp_mqtt_client_start(client);
}

#define LOG_MEM_INFO (0)

void app_main(void) {
  bsp_i2c_init();
  lv_disp_t *disp = bsp_display_start();

  // ESP_LOGI(TAG, "Display LVGL demo");
  bsp_display_lock(0);
  lv_widgets(); /* A widgets example */

  bsp_display_unlock();

  ESP_LOGI(TAG, "[APP] Startup..");
  ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
  ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

  esp_log_level_set("*", ESP_LOG_INFO);
  esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
  esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
  esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
  esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
  esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
  esp_log_level_set("outbox", ESP_LOG_VERBOSE);

  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  ESP_ERROR_CHECK(example_connect());

  mqtt_app_start();

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

    vTaskDelay(pdMS_TO_TICKS(500));
  }
#endif
}
