#include "../include/functions.h"

// ============================================================================
// Example: Temperature Functions
// ============================================================================

std::string temperature_heat(float& temperature, bool& cooling, float targetTemp) {
    temperature += 10.0f;
    cooling = false;
    std::cout << "  [temperature_heat] Heating... Temperature: " << temperature << "C" << std::endl;
    
    if (temperature < targetTemp) {
        return "heat";  // Continue heating
    }
    return "cool";  // Go to cool step
}

std::string temperature_cool(float& temperature, bool& cooling, float targetTemp) {
    temperature -= 5.0f;
    cooling = true;
    std::cout << "  [temperature_cool] Cooling... Temperature: " << temperature << "C" << std::endl;
    
    if (temperature > targetTemp) {
        return "cool";  // Continue cooling
    }
    return "process";  // Go to process step (variadic template)
}

std::string temperature_finalize(float& temperature, bool& cooling) {
    std::cout << "  [temperature_finalize] Final temperature: " << temperature << "C, cooling: " 
             << (cooling ? "YES" : "NO") << std::endl;
    return "";  // End sequence
}
// End of file