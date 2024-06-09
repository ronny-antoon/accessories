#include "fanAccessory.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <iot_button.h>

#include <on_error.hpp>

FanAccessory::FanAccessory(gpio_num_t button_pin, gpio_num_t fan_pin) {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);
  ESP_LOGV(__FILENAME__, "button_pin: %d, fan_pin: %d", button_pin, fan_pin);

  m_button_pin = button_pin;
  m_fan_pin = fan_pin;

  gpio_set_direction(m_button_pin, GPIO_MODE_INPUT);
  gpio_set_direction(m_fan_pin, GPIO_MODE_INPUT_OUTPUT);

  gpio_set_level(m_fan_pin, 0);  // TODO: Change to last known state

  button_config_t button_config = {.type = BUTTON_TYPE_GPIO,
                                   .gpio_button_config = {
                                       .gpio_num = m_button_pin,
                                       .active_level = 1,
                                   }};

  button_handle_t handle = iot_button_create(&button_config);

  // buttonCallback
  iot_button_register_cb(handle, BUTTON_PRESS_DOWN, buttonCallback, this);
}

void FanAccessory::setPower(bool power) {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);
  ESP_LOGV(__FILENAME__, "power: %d", power);

  gpio_set_level(m_fan_pin, power);
}

bool FanAccessory::getPower() const {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);
  ESP_LOGV(__FILENAME__, "return: %d", gpio_get_level(m_fan_pin));

  return gpio_get_level(m_fan_pin);
}

void FanAccessory::buttonCallback(void *button_handle, void *this_ptr) {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);

  FanAccessory *fanAccessory = static_cast<FanAccessory *>(this_ptr);

  fanAccessory->setPower(!fanAccessory->getPower());
  fanAccessory->reportAttributes();
}

void FanAccessory::identifyYourSelf() {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);

  // run task for 3 secounds to turn the fan on and off
  xTaskCreate(
      [](void *this_ptr) {
        FanAccessory *fanAccessory = static_cast<FanAccessory *>(this_ptr);
        fanAccessory->setPower(0);
        for (int i = 0; i < 3; i++) {
          fanAccessory->setPower(1);
          vTaskDelay(500 / portTICK_PERIOD_MS);
          fanAccessory->setPower(0);
          vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        vTaskDelete(nullptr);
      },
      "identify_task", 2048, this, 5, nullptr);
}