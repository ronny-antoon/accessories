#ifndef PLUGIN_ACCESSORY_HPP
#define PLUGIN_ACCESSORY_HPP

#include <hal/gpio_types.h>

#include "accessory_interface.hpp"

/**
 * @brief A plugin accessory.
 *
 * This class defines a plugin accessory.
 */
class PluginAccessory : public AccessoryInterface {
 public:
  /**
   * @brief Construct a new Plugin Accessory object.
   *
   * @param button_pin The GPIO pin number of the button.
   * @param plugin_pin The GPIO pin number of the LED.
   *
   * @note The button is assumed to be connected to the GPIO pin in pull-up mode.
   */
  PluginAccessory(gpio_num_t button_pin, gpio_num_t plugin_pin);

  /**
   * @brief Destroy the Plugin Accessory object.
   */
  ~PluginAccessory() = default;

  /**
   * @brief Set the power state of the plugin.
   *
   * @param power The power state of the plugin.
   */
  void setPower(bool power);

  /**
   * @brief Get the power state of the plugin.
   *
   * @return The power state of the plugin.
   */
  bool getPower() const;

  /**
   * @brief Identify the accessory.
   */
  void identifyYourSelf() override;

 private:
  gpio_num_t m_button_pin;  ///< The GPIO pin number of the button.
  gpio_num_t m_plugin_pin;  ///< The GPIO pin number of the LED.

  /**
   * @brief The callback function for the button.
   *
   * @param button_handle The handle of the button.
   * @param this_ptr The pointer to the PluginAccessory object.
   */
  static void buttonCallback(void *button_handle, void *this_ptr);
};

#endif  // PLUGIN_ACCESSORY_HPP