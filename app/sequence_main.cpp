#include <iostream>
#include <functional>
#include <vector>
#include "../include/SequenceFSMHelper.h"
#include "../include/functions.h"
#include "../include/foo.h"

/**
 * @file sequence_main.cpp
 * @brief Temperature simulation sequence using functions from functions.h
 */

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Temperature Sequence (Using Functions)" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    float temperature = 20.0f;
    bool cooling = false;
    Foo fooObject("SequenceWorker");  // Create Foo object with a name

    SequenceBuilder builder;

    // Step 1: Heat up using temperature_heat function
    builder.createStep("heat",
                      std::bind(temperature_heat, std::ref(temperature), std::ref(cooling), 60.0f),
                      TagType::STANDARD);

    // Step 2: Cool down using temperature_cool function
    builder.createStep("cool",
                      std::bind(temperature_cool, std::ref(temperature), std::ref(cooling), 30.0f),
                      TagType::STANDARD);

    // Step 3: Process with variadic template (Foo state value + other arguments)
    builder.createStep("process",
                      [&fooObject]() {
                          process_variadic_data("Data: ", 42, " | Foo State: ", fooObject.getState());
                          return "done";
                      },
                      TagType::STANDARD);

    // Step 4: Finalize using temperature_finalize function
    builder.createStep("done",
                      std::bind(temperature_finalize, std::ref(temperature), std::ref(cooling)),
                      TagType::STANDARD);

    std::cout << "Start: temperature=" << temperature << "C, Foo.name=" << fooObject.getName() << std::endl;
    builder.executeSequence("heat");
    std::cout << "End: temperature=" << temperature << "C, Foo.state=" << fooObject.getState() << std::endl << std::endl;

    return 0;
}
