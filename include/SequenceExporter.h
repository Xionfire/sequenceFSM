#pragma once

#include "SequenceFSMHelper.h"
#include <string>
#include <sstream>
#include <map>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

/**
 * @class SequenceExporter
 * @brief Exports a SequenceBuilder FSM sequence to JSON format
 * 
 * This class provides functionality to:
 * - Extract all steps from a SequenceBuilder
 * - Convert step information to JSON format
 * - Provide detailed or simplified JSON representations
 * - Save JSON to files in a configurable directory
 */
class SequenceExporter {
public:
    /**
     * @brief Constructor with optional output directory
     * @param builder Reference to a SequenceBuilder to export
     * @param outputDir Output directory for JSON files (default: ".json")
     */
    explicit SequenceExporter(const SequenceBuilder& builder, const std::string& outputDir = ".json")
        : m_builder(builder), m_outputDir(outputDir) {
        // Create output directory if it doesn't exist
        if (!fs::exists(m_outputDir)) {
            try {
                fs::create_directories(m_outputDir);
                std::cout << "[SequenceExporter] Created output directory: " << m_outputDir << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "[SequenceExporter] Error creating directory: " << e.what() << std::endl;
            }
        }
    }

    /**
     * @brief Destructor
     */
    ~SequenceExporter() = default;

    /**
     * @brief Sets the output directory for JSON files
     * @param outputDir Path to the output directory
     * @return true if directory was created/set successfully, false otherwise
     */
    bool setOutputDirectory(const std::string& outputDir) {
        try {
            if (!fs::exists(outputDir)) {
                fs::create_directories(outputDir);
                std::cout << "[SequenceExporter] Created output directory: " << outputDir << std::endl;
            }
            m_outputDir = outputDir;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "[SequenceExporter] Error setting output directory: " << e.what() << std::endl;
            return false;
        }
    }

    /**
     * @brief Gets the current output directory
     * @return Current output directory path
     */
    const std::string& getOutputDirectory() const {
        return m_outputDir;
    }

    /**
     * @brief Exports the sequence as a JSON string
     * @param pretty If true, formats JSON with indentation for readability
     * @return JSON string representation of the sequence
     */
    std::string exportToJSON(bool pretty = true) const {
        std::ostringstream json;
        
        if (pretty) {
            json << "{\n";
            json << "  \"sequence\": {\n";
            json << "    \"step_count\": " << m_builder.getStepCount() << ",\n";
            json << "    \"steps\": [\n";
        } else {
            json << "{\"sequence\":{\"step_count\":" << m_builder.getStepCount() 
                 << ",\"steps\":[";
        }

        // Collect all step information
        std::vector<std::pair<std::string, SequenceBuilder::StepPtr>> stepsList;
        collectSteps(stepsList);

        // Write steps to JSON
        for (size_t i = 0; i < stepsList.size(); ++i) {
            const auto& stepName = stepsList[i].first;
            const auto& step = stepsList[i].second;

            if (pretty) {
                json << "      {\n";
                json << "        \"id\": " << i << ",\n";
                json << "        \"name\": \"" << escapeJSON(stepName) << "\",\n";
                json << "        \"tag_type\": \"" 
                     << (step->getTagType() == TagType::STANDARD ? "STANDARD" : "CUSTOM") << "\",\n";
                json << "        \"tag_value\": \"" << escapeJSON(step->getTagValue()) << "\",\n";
                json << "        \"description\": \"" << escapeJSON(step->describe()) << "\"\n";
                json << "      }";
            } else {
                json << "{\"id\":" << i << ",\"name\":\"" << escapeJSON(stepName) 
                     << "\",\"tag_type\":\"" 
                     << (step->getTagType() == TagType::STANDARD ? "STANDARD" : "CUSTOM") 
                     << "\",\"tag_value\":\"" << escapeJSON(step->getTagValue()) 
                     << "\",\"description\":\"" << escapeJSON(step->describe()) << "\"}";
            }

            if (i < stepsList.size() - 1) {
                json << ",";
            }
            if (pretty) {
                json << "\n";
            }
        }

        if (pretty) {
            json << "    ]\n";
            json << "  }\n";
            json << "}";
        } else {
            json << "]}}}";
        }

        return json.str();
    }

    /**
     * @brief Exports a simplified JSON with only step names and types
     * @return Simplified JSON string
     */
    std::string exportSimplifiedJSON() const {
        std::ostringstream json;
        json << "{\n";
        json << "  \"steps\": [\n";

        std::vector<std::pair<std::string, SequenceBuilder::StepPtr>> stepsList;
        collectSteps(stepsList);

        for (size_t i = 0; i < stepsList.size(); ++i) {
            const auto& stepName = stepsList[i].first;
            const auto& step = stepsList[i].second;

            json << "    {\n";
            json << "      \"name\": \"" << escapeJSON(stepName) << "\",\n";
            json << "      \"type\": \"" 
                 << (step->getTagType() == TagType::STANDARD ? "STANDARD" : "CUSTOM") << "\"\n";
            json << "    }";

            if (i < stepsList.size() - 1) {
                json << ",\n";
            } else {
                json << "\n";
            }
        }

        json << "  ]\n";
        json << "}";

        return json.str();
    }

    /**
     * @brief Saves the detailed JSON to a file
     * @param filename Name of the file to save (without directory)
     * @param pretty If true, formats JSON with indentation
     * @return true if save was successful, false otherwise
     */
    bool saveToFile(const std::string& filename, bool pretty = true) const {
        try {
            fs::path filePath = fs::path(m_outputDir) / filename;
            std::ofstream outFile(filePath);
            
            if (!outFile.is_open()) {
                std::cerr << "[SequenceExporter] Failed to open file for writing: " << filePath << std::endl;
                return false;
            }

            outFile << exportToJSON(pretty);
            outFile.close();

            std::cout << "[SequenceExporter] Successfully saved to: " << filePath << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "[SequenceExporter] Error saving file: " << e.what() << std::endl;
            return false;
        }
    }

    /**
     * @brief Saves the simplified JSON to a file
     * @param filename Name of the file to save (without directory)
     * @return true if save was successful, false otherwise
     */
    bool saveSimplifiedToFile(const std::string& filename) const {
        try {
            fs::path filePath = fs::path(m_outputDir) / filename;
            std::ofstream outFile(filePath);
            
            if (!outFile.is_open()) {
                std::cerr << "[SequenceExporter] Failed to open file for writing: " << filePath << std::endl;
                return false;
            }

            outFile << exportSimplifiedJSON();
            outFile.close();

            std::cout << "[SequenceExporter] Successfully saved to: " << filePath << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "[SequenceExporter] Error saving file: " << e.what() << std::endl;
            return false;
        }
    }

    /**
     * @brief Saves both detailed and simplified JSON to files
     * @param detailedFilename Name for the detailed JSON file
     * @param simplifiedFilename Name for the simplified JSON file
     * @return true if both saves were successful, false otherwise
     */
    bool saveBothToFiles(const std::string& detailedFilename, const std::string& simplifiedFilename) const {
        bool detailed = saveToFile(detailedFilename, true);
        bool simplified = saveSimplifiedToFile(simplifiedFilename);
        return detailed && simplified;
    }

private:
    const SequenceBuilder& m_builder;  ///< Reference to the SequenceBuilder to export
    std::string m_outputDir;           ///< Output directory for JSON files

    /**
     * @brief Collects all steps from the builder
     * @param stepsList Output vector of step pairs
     */
    void collectSteps(std::vector<std::pair<std::string, SequenceBuilder::StepPtr>>& stepsList) const {
        const auto& stepsMap = m_builder.getStepsMap();
        for (const auto& pair : stepsMap) {
            stepsList.push_back(pair);
        }
    }

    /**
     * @brief Escapes special characters for JSON string values
     * @param str Input string
     * @return Escaped string safe for JSON
     */
    std::string escapeJSON(const std::string& str) const {
        std::ostringstream o;
        for (char c : str) {
            switch (c) {
                case '"':  o << "\\\""; break;
                case '\\': o << "\\\\"; break;
                case '\b': o << "\\b";  break;
                case '\f': o << "\\f";  break;
                case '\n': o << "\\n";  break;
                case '\r': o << "\\r";  break;
                case '\t': o << "\\t";  break;
                default:
                    if ('\x00' <= c && c <= '\x1f') {
                        o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                    } else {
                        o << c;
                    }
            }
        }
        return o.str();
    }
};
