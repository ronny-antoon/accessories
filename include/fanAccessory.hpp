#ifndef FAN_ACCESSORY_HPP
#define FAN_ACCESSORY_HPP

#include <hal/gpio_types.h>

#include "accessory_interface.hpp"

/**
 * @brief A fan accessory.
 *
 * This class defines a fan accessory.
 */
class FanAccessory : public AccessoryInterface {
 public:
  /**
   * @brief Construct a new Fan Accessory object.
   *
   * @param button_pin The GPIO pin number of the button.
   * @param fan_pin The GPIO pin number of the fan.
   *
   * @note The button is assumed to be connected to the GPIO pin in pull-up mode.
   */
  FanAccessory(gpio_num_t button_pin, gpio_num_t fan_pin);

  /**
   * @brief Destroy the Fan Accessory object.
   */
  ~FanAccessory() = default;

  /**
   * @brief Set the power state of the fan.
   *
   * @param power The power state of the fan.
   */
  void setPower(bool power);

  /**
   * @brief Get the power state of the fan.
   *
   * @return The power state of the fan.
   */
  bool getPower() const;

  /**
   * @brief Identify the accessory.
   */
  void identifyYourSelf() override;

 private:
  gpio_num_t m_button_pin;  ///< The GPIO pin number of the button.
  gpio_num_t m_fan_pin;     ///< The GPIO pin number of the fan.

  /**
   * @brief The callback function for the button.
   *
   * @param button_handle The handle of the button.
   * @param this_ptr The pointer to the FanAccessory object.
   */
  static void buttonCallback(void *button_handle, void *this_ptr);
};

#endif  // FAN_ACCESSORY_HPP