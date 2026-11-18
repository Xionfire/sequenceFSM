#include "../include/foo.h"
#include "../include/device.h"
#include "../include/SequenceExporter.h"
#include <iostream>
#include <memory>

/**
 * @brief Example: Export Foo workflow sequence to JSON and save to file
 * 
 * Demonstrates how to:
 * 1. Create a Foo workflow with a Device
 * 2. Initialize the FSM sequence
 * 3. Export the sequence to JSON format
 * 4. Save JSON to files in a configurable directory
 * 5. Display the JSON output
 */
int main() {
    std::cout << "========== Sequence Exporter with File Saving ==========\n" << std::endl;

    // Step 1: Create a SimpleDevice
    auto device = std::make_shared<SimpleDevice>("MyDevice");
    std::cout << "Device created: " << device->getName() << std::endl;

    // Step 2: Create a Foo workflow with the device
    Foo workflow("MyWorkflow", device);
    std::cout << "Workflow created with device\n" << std::endl;

    // Step 3: Initialize the workflow sequence
    workflow.initializeSequence();
    std::cout << "Sequence initialized\n" << std::endl;

    // Step 4: Create an exporter with custom output directory
    std::cout << "--- Creating Exporter ---" << std::endl;
    SequenceExporter exporter(workflow, ".json");  // Default output directory

    // Step 5: Export and save detailed JSON
    std::cout << "--- Exporting and Saving JSON ---" << std::endl;
    exporter.saveToFile("workflow_detailed.json", true);
    exporter.saveSimplifiedToFile("workflow_simplified.json");
    std::cout << std::endl;

    return 0;
}
