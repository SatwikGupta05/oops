#pragma once
#include "FileHandler.h"
#include "PredictionAlgorithm.h"
#include <memory>
#include <map>
#include <stdexcept>

class StockPredictor {
private:
    std::unique_ptr<FileHandler> fileHandler;
    std::map<std::string, std::unique_ptr<PredictionAlgorithm>> algorithms;

public:
    explicit StockPredictor(const std::string& dataDir);

    // Core operations
    std::vector<StockData> getHistoricalData(const std::string& symbol);
    std::vector<double> predict(const std::string& symbol, const std::string& algorithm);
    std::vector<std::string> getAvailableAlgorithms() const;

    // Algorithm management
    void registerAlgorithm(const std::string& name, std::unique_ptr<PredictionAlgorithm> algorithm);
    
    // Utility methods
    std::string getDataDirectory() const;

private:
    void initializeAlgorithms();
};