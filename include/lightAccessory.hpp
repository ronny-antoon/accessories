#ifndef LIGHT_ACCESSORY_HPP
#define LIGHT_ACCESSORY_HPP

#include <hal/gpio_types.h>

#include "accessory_interface.hpp"

/**
 * @brief A light accessory.
 *
 * This class defines a light accessory.
 */
class LightAccessory : public AccessoryInterface {
 public:
  /**
   * @brief Construct a new Light Accessory object.
   *
   * @param button_pin The GPIO pin number of the button.
   * @param led_pin The GPIO pin number of the LED.
   *
   * @note The button is assumed to be connected to the GPIO pin in pull-up mode.
   */
  LightAccessory(gpio_num_t button_pin, gpio_num_t led_pin);

  /**
   * @brief Destroy the Light Accessory object.
   */
  ~LightAccessory() = default;

  /**
   * @brief Set the power state of the light.
   *
   * @param power The power state of the light.
   */
  void setPower(bool power);

  /**
   * @brief Get the power state of the light.
   *
   * @return The power state of the light.
   */
  bool getPower() const;

  /**
   * @brief Identify the accessory.
   */
  void identifyYourSelf() override;

 private:
  gpio_num_t m_button_pin;  ///< The GPIO pin number of the button.
  gpio_num_t m_led_pin;     ///< The GPIO pin number of the LED.

  /**
   * @brief The callback function for the button.
   *
   * @param button_handle The handle of the button.
   * @param this_ptr The pointer to the LightAccessory object.
   */
  static void buttonCallback(void *button_handle, void *this_ptr);
};

#endif  // LIGHT_ACCESSORY_HPP