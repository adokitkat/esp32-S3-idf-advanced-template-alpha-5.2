#include "system_.hpp" // Class structure and variables

#include "esp_check.h"

//
// NOTE: We are keeping all variable names to 15 characters or less so that the variables names can also
// be used as key values in non-volitile storage.
//

/* External Variables */
extern SemaphoreHandle_t semNVSEntry;

bool System::restoreVariablesFromNVS()
{
    if (show & _showNVS)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "()");

    uint8_t temp = 0;

    if (nvs == nullptr)
        nvs = NVS::getInstance(); // First, get the nvs object handle if didn't do this previously.

    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY) == pdTRUE)
    {
        if (show & _showNVS)
            routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): openNVStorage 'system'");

        if (!nvs->openNVSStorage("system", true))
        {
            ESP_LOGE(TAG, "Error, Unable to OpenNVStorage inside restoreVariablesFromNVS");
            xSemaphoreGive(semNVSEntry);
            return false;
        }
    }

    bool successFlag = true;

    if (show & _showNVS)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): system start");

    if (successFlag) // Restore runStackSizeK
    {
        temp = runStackSizeK; // This will save the default size if value doesn't exist yet in nvs.

        if (nvs->getU8IntegerFromNVS("runStackSizeK", &temp))
        {
            if (temp > runStackSizeK) // Ok to use any value greater than the default size.
            {
                runStackSizeK = temp;
                saveToNVSDelaySecs = 2; // Save it
            }

            if (show & _showNVS)
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): runStackSizeK is " + std::to_string(runStackSizeK));
        }
        else
        {
            successFlag = false;
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Error, Unable to restore runStackSizeK");
        }
    }

    if (successFlag) // Restore gpioStackSizeK
    {
        temp = gpioStackSizeK; // This will save the default size if value doesn't exist yet in nvs.

        if (nvs->getU8IntegerFromNVS("gpioStackSizeK", &temp))
        {
            if (temp > gpioStackSizeK) // Ok to use any value greater than the default size.
            {
                gpioStackSizeK = temp;
                saveToNVSDelaySecs = 2; // Save it
            }

            if (show & _showNVS)
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): gpioStackSizeK is " + std::to_string(gpioStackSizeK));
        }
        else
        {
            successFlag = false;
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Error, Unable to restore gpioStackSizeK");
        }
    }

    if (successFlag) // Restore timerStackSizeK
    {
        temp = timerStackSizeK; // This will save the default size if value doesn't exist yet in nvs.

        if (nvs->getU8IntegerFromNVS("timerStackSizeK", &temp))
        {
            if (temp > timerStackSizeK) // Ok to use any value greater than the default size.
            {
                timerStackSizeK = temp;
                saveToNVSDelaySecs = 2; // Save it
            }

            if (show & _showNVS)
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): timerStackSizeK is " + std::to_string(timerStackSizeK));
        }
        else
        {
            successFlag = false;
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Error, Unable to restore timerStackSizeK");
        }
    }

    if (successFlag) // Restore bootCount
    {
        if (nvs->getU32IntegerFromNVS("bootCount", &bootCount))
        {
            if (show & _showNVS)
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): bootCount is " + std::to_string(bootCount));
        }
        else
            successFlag = false;
    }

    if (show & _showNVS)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): system end");

    if (successFlag)
    {
        if (show & _showNVS)
            routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): Succeeded");

        nvs->closeNVStorage(true); // Commit changes
    }
    else
    {
        ESP_LOGE(TAG, "restoreVariablesFromNVS Failed");
        nvs->closeNVStorage(false); // No changes
    }

    xSemaphoreGive(semNVSEntry);
    return successFlag;
}

bool System::saveVariablesToNVS()
{
    if (show & _showNVS)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "():");
    //
    // The best idea is to save only changed values.  Right now, we try to save everything.
    // The NVS object we call will avoid over-writing variables which already hold the correct value.
    // Later, we may try to add and track 'dirty' bits to avoid trying to save a value that hasn't changed.
    //
    if (show & _showNVS)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): system start");
    else
    {
        if (xSemaphoreTake(semNVSEntry, portMAX_DELAY) == pdTRUE)
        {
            if (!nvs->openNVSStorage("system", true)) // Read/Write
            {
                ESP_LOGE(TAG, "Error, Unable to OpenNVStorage inside saveVariablesToNVS");
                xSemaphoreGive(semNVSEntry);
                return false;
            }
        }
    }

    bool successFlag = true;

    if (successFlag) // Save runStackSizeK
    {
        if (nvs->saveU8IntegerToNVS("runStackSizeK", runStackSizeK))
        {
            if (show & _showNVS)
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): runStackSizeK = " + std::to_string(runStackSizeK));
        }
        else
        {
            ESP_LOGE(TAG, "Error, Unable to saveU8IntegerToNVS runStackSizeK");
            successFlag = false;
        }
    }

    if (successFlag) // Save gpioStackSizeK
    {
        if (nvs->saveU8IntegerToNVS("gpioStackSizeK", gpioStackSizeK))
        {
            if (show & _showNVS)
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): gpioStackSizeK = " + std::to_string(gpioStackSizeK));
        }
        else
        {
            ESP_LOGE(TAG, "Error, Unable to saveU8IntegerToNVS gpioStackSizeK");
            successFlag = false;
        }
    }

    if (successFlag) // Save timerStackSizeK
    {
        if (nvs->saveU8IntegerToNVS("timerStackSizeK", timerStackSizeK))
        {
            if (show & _showNVS)
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): timerStackSizeK = " + std::to_string(timerStackSizeK));
        }
        else
        {
            ESP_LOGE(TAG, "Error, Unable to saveU8IntegerToNVS timerStackSizeK");
            successFlag = false;
        }
    }

    if (successFlag) // Save bootCount
    {
        if (nvs->saveU32IntegerToNVS("bootCount", bootCount))
        {
            if (show & _showNVS)
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): bootCount = " + std::to_string(bootCount));
        }
        else
        {
            ESP_LOGE(TAG, "Error, Unable to saveU32IntegerToNVS bootCount");
            successFlag = false;
        }
    }

    if (show & _showNVS)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): system end");

    if (successFlag)
    {
        if (show & _showNVS)
            ESP_LOGE(TAG, "saveVariablesToNVS Succeeded");
        nvs->closeNVStorage(true); // Commit changes
    }
    else if (show & _showNVS)
    {
        ESP_LOGE(TAG, "saveVariablesToNVS Failed");
        nvs->closeNVStorage(false); // Discard changes
    }

    xSemaphoreGive(semNVSEntry);
    return successFlag;
}
