#include "windowAccessory.hpp"

#include <esp_timer.h>
#include <iot_button.h>

#include <on_error.hpp>

WindowAccessory::WindowAccessory(gpio_num_t button_down_pin, gpio_num_t button_up_pin,
                                 gpio_num_t motor_up_pin, gpio_num_t motor_down_pin, uint32_t time_to_open,
                                 uint32_t time_to_close) {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);
  ESP_LOGV(__FILENAME__,
           "button_down_pin: %d, button_up_pin: %d, motor_up_pin: %d, motor_down_pin: %d, time_to_open: %d, "
           "time_to_close: %d",
           button_down_pin, button_up_pin, motor_up_pin, motor_down_pin, (int)time_to_open,
           (int)time_to_close);

  m_button_down_pin = button_down_pin;
  m_button_up_pin = button_up_pin;
  m_motor_up_pin = motor_up_pin;
  m_motor_down_pin = motor_down_pin;

  m_time_to_open = time_to_open;
  m_time_to_close = time_to_close;

  gpio_set_direction(m_button_down_pin, GPIO_MODE_INPUT);
  gpio_set_direction(m_button_up_pin, GPIO_MODE_INPUT);
  gpio_set_direction(m_motor_up_pin, GPIO_MODE_OUTPUT);
  gpio_set_direction(m_motor_down_pin, GPIO_MODE_OUTPUT);

  gpio_set_level(m_motor_up_pin, 0);
  gpio_set_level(m_motor_down_pin, 0);

  button_config_t button_down_config = {.type = BUTTON_TYPE_GPIO,
                                        .gpio_button_config = {
                                            .gpio_num = m_button_down_pin,
                                            .active_level = 1,
                                        }};

  button_handle_t handle_down = iot_button_create(&button_down_config);

  button_config_t button_up_config = {.type = BUTTON_TYPE_GPIO,
                                      .gpio_button_config = {
                                          .gpio_num = m_button_up_pin,
                                          .active_level = 1,
                                      }};

  button_handle_t handle_up = iot_button_create(&button_up_config);

  iot_button_register_cb(handle_down, BUTTON_PRESS_DOWN, buttonDownCallback, this);
  iot_button_register_cb(handle_up, BUTTON_PRESS_DOWN, buttonUpCallback, this);

  m_moveBlindToTask_handle = nullptr;
}

void WindowAccessory::buttonUpCallback(void *button_handle, void *this_ptr) {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);

  WindowAccessory *windowAccessory = static_cast<WindowAccessory *>(this_ptr);

  if (windowAccessory->m_current_position != windowAccessory->m_target_position) {
    windowAccessory->moveBlindTo(windowAccessory->m_current_position);
  } else {
    windowAccessory->moveBlindTo(100);
  }

  windowAccessory->reportAttributes();
}

void WindowAccessory::buttonDownCallback(void *button_handle, void *this_ptr) {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);

  WindowAccessory *windowAccessory = static_cast<WindowAccessory *>(this_ptr);

  if (windowAccessory->m_current_position != windowAccessory->m_target_position) {
    windowAccessory->moveBlindTo(windowAccessory->m_current_position);
  } else {
    windowAccessory->moveBlindTo(0);
  }

  windowAccessory->reportAttributes();
}

void WindowAccessory::startMoveBlind(MoveDirection direction) {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);
  ESP_LOGV(__FILENAME__, "direction: %d", (int)direction);

  if (direction == MoveDirection::UP) {
    gpio_set_level(m_motor_up_pin, 1);
    gpio_set_level(m_motor_down_pin, 0);
  } else {
    gpio_set_level(m_motor_up_pin, 0);
    gpio_set_level(m_motor_down_pin, 1);
  }

  m_timer = esp_timer_get_time();
}

void WindowAccessory::stopMoveBlind() {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);

  gpio_set_level(m_motor_up_pin, 0);
  gpio_set_level(m_motor_down_pin, 0);

  ESP_LOGV(__FILENAME__, "Time taken to move blind: %f", ((esp_timer_get_time() - m_timer) / 1000000));
}

bool WindowAccessory::targetPositionReached(MoveDirection direction) {
  if (direction == MoveDirection::UP) {
    return m_current_position >= m_target_position;
  } else {
    return m_current_position <= m_target_position;
  }
}

void WindowAccessory::moveBlindTo(uint8_t position) {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);
  ESP_LOGV(__FILENAME__, "position: %d", position);

  if (position > 100) {
    position = 100;
  }
  if (position < 0) {
    position = 0;
  }

  if (m_moveBlindToTask_handle != nullptr) {
    vTaskDelete(m_moveBlindToTask_handle);
    m_moveBlindToTask_handle = nullptr;
  }

  this->m_target_position = position;

  xTaskCreate(
      [](void *this_ptr) {
        WindowAccessory *windowAccessory = static_cast<WindowAccessory *>(this_ptr);
        windowAccessory->moveBlindToTargetTask();
        windowAccessory->reportAttributes();
        windowAccessory->m_moveBlindToTask_handle = nullptr;
        vTaskDelete(nullptr);
      },
      "moveBlindToTask", 10000, this, 5, &m_moveBlindToTask_handle);
}

void WindowAccessory::identifyYourSelf() { ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__); }

void WindowAccessory::moveBlindToTargetTask() {
  ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);

  double currentPosition = m_current_position;

  // Ensure target position is within valid range
  if (m_target_position > 100) {
    m_target_position = 100;
  }
  if (m_target_position < 0) {
    m_target_position = 0;
  }

  // Return if the blind is already at the target position
  if (m_current_position == m_target_position) {
    stopMoveBlind();
    return;
  }

  MoveDirection direction = m_current_position < m_target_position ? MoveDirection::UP : MoveDirection::DOWN;
  startMoveBlind(direction);

  uint8_t startPostion = m_current_position;
  bool updateFirstFlag = true;

  while (!targetPositionReached(direction)) {
    vTaskDelay(m_checkInterval / portTICK_PERIOD_MS);

    // Update the current position based on the movement direction
    currentPosition += (direction == MoveDirection::UP)
                           ? (double)m_checkInterval / (m_time_to_open * 1000) * 100
                           : -(double)m_checkInterval / (m_time_to_close * 1000) * 100;
    m_current_position = (uint8_t)currentPosition;

    // report attributes if the position has changed by 1% or more only once
    if (updateFirstFlag && abs(startPostion - m_current_position) >= 1) {
      reportAttributes();
      updateFirstFlag = false;
    }
  }

  stopMoveBlind();

  m_current_position = m_target_position;

  reportAttributes();
}