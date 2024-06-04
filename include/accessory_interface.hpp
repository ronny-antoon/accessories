#ifndef ACCESSORY_INTERFACE_HPP
#define ACCESSORY_INTERFACE_HPP

/**
 * @brief Interface for an accessory.
 *
 * This class defines the interface for an accessory device.
 */
class AccessoryInterface
{
public:
    /**
     * @brief default destructor
     */
    virtual ~AccessoryInterface() = default;

    /**
     * @brief Set the callback function for reporting attributes.
     *
     * @param reportAttributesCallback The callback function to be called for reporting attributes.
     * @param callbackParameter The parameter to be passed to the callback function.
     */
    void setReportAttributesCallback(void (*reportAttributesCallback)(void *), void *callbackParameter);

    /**
     * @brief Report the attributes of the accessory.
     */
    void reportAttributes();

    /**
     * @brief Identify the accessory.
     */
    virtual void identifyYourSelf() = 0;

private:
    void (*m_reportAttributesCallback)(void *) = nullptr; ///< The callback function for reporting attributes.
    void *m_callbackParameter = nullptr;                  ///< The parameter to be passed to the callback function.
};

#endif // ACCESSORY_INTERFACE_HPP
