#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <sstream>

/**
 * @file functions.h
 * @brief Collection of step functions for use in sequences
 * 
 * These functions can be used with SequenceBuilder via std::bind or functors.
 * Each function takes parameters and returns the name of the next step.
 */

// ============================================================================
// FooSimple Class: Demonstrates compute() method call in template
// ============================================================================

class FooSimple {
public:
    int value = 0;
    
    FooSimple() : value(0) {}
    FooSimple(int v) : value(v) {}
    
    void compute() {
        value += 100;
        std::cout << "      [FooSimple::compute()] Called - value increased to " << value << std::endl;
    }
    
    void print() const {
        std::cout << "      [FooSimple::print()] Value = " << value;
    }
};

// ============================================================================
// Example: Temperature Functions
// ============================================================================

std::string temperature_heat(float& temperature, bool& cooling, float targetTemp);
std::string temperature_cool(float& temperature, bool& cooling, float targetTemp);
std::string temperature_finalize(float& temperature, bool& cooling);

// ============================================================================
// Template Function: Variable Arguments (C++17 Variadic Template)
// ============================================================================

/**
 * @brief Generic template function that accepts any number of arguments of any type
 * @param args Variable number of arguments of any type
 * @return Name of next step
 * 
 * Uses C++17 fold expressions to process all arguments dynamically
 * No std::string dependency - pure template variadic arguments
 * 
 * Special handling: If an argument is of type FooSimple, calls its compute() method
 */
template<typename... Args>
std::string process_variadic_data(Args&&... args) {
    std::cout << "  [process_variadic_data] Processing " << sizeof...(args) << " arguments: ";
    
    // Use fold expression with comma operator to process each argument
    (..., (
        // Check if argument is of type FooSimple using if constexpr (C++17)
        [&args](){
            if constexpr (std::is_same_v<std::decay_t<decltype(args)>, FooSimple>) {
                std::cout << " [FooSimple object] ";
                args.compute();
                args.print();
            } else {
                std::cout << args;
            }
        }()
    ));
    
    std::cout << std::endl;
    return "";
}