#ifndef WINDOW_ACCESSORY_HPP
#define WINDOW_ACCESSORY_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <hal/gpio_types.h>

#include <cstdint>

#include "accessory_interface.hpp"

/**
 * @brief A window accessory.
 *
 * This class defines a window accessory.
 */
class WindowAccessory : public AccessoryInterface {
 public:
  enum class MoveDirection { UP, DOWN };
  /**
   * @brief Construct a new Window Accessory object.
   *
   * @param button_down_pin The GPIO pin number of the down button.
   * @param button_up_pin The GPIO pin number of the up button.
   * @param motor_up_pin The GPIO pin number of the motor up.
   * @param motor_down_pin The GPIO pin number of the motor down.
   * @param time_to_open The time in seconds need to fully open the window.
   * @param time_to_close The time in seconds need to fully close the window.
   *
   */
  WindowAccessory(gpio_num_t button_down_pin, gpio_num_t button_up_pin, gpio_num_t motor_up_pin,
                  gpio_num_t motor_down_pin, uint32_t time_to_open, uint32_t time_to_close);

  /**
   * @brief Destroy the Window Accessory object.
   */
  ~WindowAccessory() = default;

  void startMoveBlind(MoveDirection direction);  //< Start moving the blind up/down.
  void stopMoveBlind();                          ///< Stop moving the blind.

  bool targetPositionReached(
      MoveDirection direction);  ///< Check if the blind has reached the target position.

  void moveBlindToTargetTask();  ///< Task to move the blind to a specific position.

  /**
   * @brief Identify the accessory.
   */
  void identifyYourSelf() override;

  uint16_t getCurrentPosition() { return m_current_position; }  ///< Get the current position of the blind.
  uint16_t getTargetPosition() { return m_target_position; }    ///< Get the target position of the blind.

  /**
   * @brief Move the blind to a specific position.
   *
   * @param position The target position for the blind.
   */
  void moveBlindTo(uint8_t position);

 private:
  gpio_num_t m_button_down_pin;  ///< The GPIO pin number of the down button.
  gpio_num_t m_button_up_pin;    ///< The GPIO pin number of the up button.
  gpio_num_t m_motor_up_pin;     ///< The GPIO pin number of the motor up.
  gpio_num_t m_motor_down_pin;   ///< The GPIO pin number of the motor down.

  uint32_t m_time_to_open;   ///< The time in seconds need to fully open the window.
  uint32_t m_time_to_close;  ///< The time in seconds need to fully close the window.

  uint8_t m_target_position;   ///< The target position of the blind.
  uint8_t m_current_position;  ///< The current position of the blind.

  TaskHandle_t m_moveBlindToTask_handle;  ///< Task handle for the moveBlindToTask.
  uint8_t m_checkInterval;                ///< Interval to check the postion when the blind moving. in ms.

  float m_timer = 0;
  /**
   * @brief The callback function for the down button.
   *
   * @param button_handle The handle of the button.
   * @param this_ptr The pointer to the WindowAccessory object.
   */
  static void buttonDownCallback(void *button_handle, void *this_ptr);

  /**
   * @brief The callback function for the up button.
   *
   * @param button_handle The handle of the button.
   * @param this_ptr The pointer to the WindowAccessory object.
   */
  static void buttonUpCallback(void *button_handle, void *this_ptr);
};

#endif  // WINDOW_ACCESSORY_HPP