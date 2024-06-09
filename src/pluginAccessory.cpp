#include "pluginAccessory.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <iot_button.h>

#include <on_error.hpp>

PluginAccessory::PluginAccessory(gpio_num_t button_pin, gpio_num_t plugin_pin) {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);
  ESP_LOGV(__FILENAME__, "button_pin: %d, plugin_pin: %d", button_pin, plugin_pin);

  m_button_pin = button_pin;
  m_plugin_pin = plugin_pin;

  gpio_set_direction(m_button_pin, GPIO_MODE_INPUT);
  gpio_set_direction(m_plugin_pin, GPIO_MODE_INPUT_OUTPUT);

  gpio_set_level(m_plugin_pin, 0);  // TODO: Change to last known state

  button_config_t button_config = {.type = BUTTON_TYPE_GPIO,
                                   .gpio_button_config = {
                                       .gpio_num = m_button_pin,
                                       .active_level = 1,
                                   }};

  button_handle_t handle = iot_button_create(&button_config);

  // buttonCallback
  iot_button_register_cb(handle, BUTTON_PRESS_DOWN, buttonCallback, this);
}

void PluginAccessory::setPower(bool power) {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);
  ESP_LOGV(__FILENAME__, "power: %d", power);

  gpio_set_level(m_plugin_pin, power);
}

bool PluginAccessory::getPower() const {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);
  ESP_LOGV(__FILENAME__, "return: %d", gpio_get_level(m_plugin_pin));

  return gpio_get_level(m_plugin_pin);
}

void PluginAccessory::buttonCallback(void *button_handle, void *this_ptr) {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);

  PluginAccessory *pluginAccessory = static_cast<PluginAccessory *>(this_ptr);

  pluginAccessory->setPower(!pluginAccessory->getPower());
  pluginAccessory->reportAttributes();
}

void PluginAccessory::identifyYourSelf() {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);

  // Turn on off plugin 3 seconds
  xTaskCreate(
      [](void *this_ptr) {
        PluginAccessory *pluginAccessory = static_cast<PluginAccessory *>(this_ptr);
        pluginAccessory->setPower(0);
        for (int i = 0; i < 3; i++) {
          pluginAccessory->setPower(1);
          vTaskDelay(500 / portTICK_PERIOD_MS);
          pluginAccessory->setPower(0);
          vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        vTaskDelete(nullptr);
      },
      "plugin_identify_task", 2048, this, 5, nullptr);
}