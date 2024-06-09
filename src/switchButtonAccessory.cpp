#include "switchButtonAccessory.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <iot_button.h>

#include <on_error.hpp>

SwitchButtonAccessory::SwitchButtonAccessory(gpio_num_t button_pin) {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);
  ESP_LOGV(__FILENAME__, "button_pin: %d", button_pin);

  m_button_pin = button_pin;

  gpio_set_direction(m_button_pin, GPIO_MODE_INPUT);

  button_config_t button_config = {.type = BUTTON_TYPE_GPIO,
                                   .long_press_time = 2000,
                                   .short_press_time = 80,
                                   .gpio_button_config = {
                                       .gpio_num = m_button_pin,
                                       .active_level = 1,
                                   }};

  button_handle_t handle = iot_button_create(&button_config);

  // buttonCallback
  iot_button_register_cb(
      handle, BUTTON_SINGLE_CLICK,
      [](void *button_handle, void *this_ptr) {
        ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);

        SwitchButtonAccessory *switchButtonAccessory = static_cast<SwitchButtonAccessory *>(this_ptr);

        switchButtonAccessory->m_last_press_type = SwitchButtonAccessory::PressType::SinglePress;
        switchButtonAccessory->reportAttributes();
      },
      this);

  iot_button_register_cb(
      handle, BUTTON_LONG_PRESS_START,
      [](void *button_handle, void *this_ptr) {
        ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);

        SwitchButtonAccessory *switchButtonAccessory = static_cast<SwitchButtonAccessory *>(this_ptr);

        switchButtonAccessory->m_last_press_type = SwitchButtonAccessory::PressType::LongPress;
        switchButtonAccessory->reportAttributes();
      },
      this);

  iot_button_register_cb(
      handle, BUTTON_DOUBLE_CLICK,
      [](void *button_handle, void *this_ptr) {
        ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);

        SwitchButtonAccessory *switchButtonAccessory = static_cast<SwitchButtonAccessory *>(this_ptr);

        switchButtonAccessory->m_last_press_type = SwitchButtonAccessory::PressType::DoublePress;
        switchButtonAccessory->reportAttributes();
      },
      this);
}

void SwitchButtonAccessory::identifyYourSelf() { ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__); }

SwitchButtonAccessory::PressType SwitchButtonAccessory::getLastPressType() const {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);
  ESP_LOGV(__FILENAME__, "return: %d", m_last_press_type);

  return m_last_press_type;
}