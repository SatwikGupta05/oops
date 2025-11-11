#include "../include/FileHandler.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>

FileHandler::FileHandler(const std::string& directory) : dataDirectory(directory) {}

std::vector<StockData> FileHandler::readStockData(const std::string& symbol) {
    std::vector<StockData> data;
    std::string filePath = buildFilePath(symbol);
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    std::string line;
    // Skip header
    std::getline(file, line);
    
    while (std::getline(file, line)) {
        auto parts = splitCSVLine(line);
        if (validateDataEntry(parts)) {
            data.emplace_back(
                symbol,
                parts[0],  // date
                std::stod(parts[1]),  // open
                std::stod(parts[2]),  // high
                std::stod(parts[3]),  // low
                std::stod(parts[4]),  // close
                std::stod(parts[5])   // volume
            );
        }
    }

    return data;
}

void FileHandler::writePredictions(const std::string& symbol, const std::vector<double>& predictions) {
    std::string filePath = buildFilePath(symbol, true);
    std::ofstream file(filePath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not create file: " + filePath);
    }

    file << "Date,Predicted_Close\n";
    // TODO: Add date generation logic
    for (const auto& pred : predictions) {
        file << "YYYY-MM-DD," << pred << "\n";
    }
}

std::vector<std::string> FileHandler::splitCSVLine(const std::string& line) {
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string item;

    while (std::getline(ss, item, ',')) {
        result.push_back(item);
    }

    return result;
}

bool FileHandler::validateCSVFormat(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    std::string header;
    std::getline(file, header);
    
    // Expected format: Date,Open,High,Low,Close,Volume
    return header.find("Date") != std::string::npos &&
           header.find("Open") != std::string::npos &&
           header.find("High") != std::string::npos &&
           header.find("Low") != std::string::npos &&
           header.find("Close") != std::string::npos &&
           header.find("Volume") != std::string::npos;
}

bool FileHandler::validateDataEntry(const std::vector<std::string>& entry) {
    if (entry.size() != 6) return false;

    try {
        // Skip date validation for now (entry[0])
        for (size_t i = 1; i < entry.size(); ++i) {
            std::stod(entry[i]);
        }
        return true;
    } catch (...) {
        return false;
    }
}

std::string FileHandler::buildFilePath(const std::string& symbol, bool isPrediction) {
    std::filesystem::path path(dataDirectory);
    if (isPrediction) {
        path /= (symbol + "_predictions.csv");
    } else {
        path /= (symbol + ".csv");
    }
    return path.string();
}