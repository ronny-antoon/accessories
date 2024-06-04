#include "accessory_interface.hpp"

#include <on_error.hpp>

void AccessoryInterface::setReportAttributesCallback(void (*reportAttributesCallback)(void *), void *callbackParameter)
{
    ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);
    m_reportAttributesCallback = reportAttributesCallback;
    m_callbackParameter = callbackParameter;
}

void AccessoryInterface::reportAttributes()
{
    ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);
    if (m_reportAttributesCallback)
    {
        m_reportAttributesCallback(m_callbackParameter);
    }
    else
    {
        ON_ERR_PRINT(ESP_ERR_INVALID_ARG, "m_reportAttributesCallback is null");
    }
}