#pragma once
#include "Stock.h"
#include <string>
#include <vector>
#include <memory>

class FileHandler {
private:
    std::string dataDirectory;

public:
    explicit FileHandler(const std::string& directory);

    // File operations
    std::vector<StockData> readStockData(const std::string& symbol);
    void writePredictions(const std::string& symbol, const std::vector<double>& predictions);

    // Validation
    bool validateCSVFormat(const std::string& filePath);
    bool validateDataEntry(const std::vector<std::string>& entry);
    
    // Getters
    std::string getDataDirectory() const { return dataDirectory; }

private:
    std::vector<std::string> splitCSVLine(const std::string& line);
    std::string buildFilePath(const std::string& symbol, bool isPrediction = false);
};