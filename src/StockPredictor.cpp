#include "../include/StockPredictor.h"

StockPredictor::StockPredictor(const std::string& dataDir) 
    : fileHandler(std::make_unique<FileHandler>(dataDir)) {
    initializeAlgorithms();
}

void StockPredictor::initializeAlgorithms() {
    registerAlgorithm("SMA", std::make_unique<MovingAverageAlgorithm>(5));
    registerAlgorithm("EMA", std::make_unique<ExponentialMovingAverageAlgorithm>(0.2));
}

std::vector<StockData> StockPredictor::getHistoricalData(const std::string& symbol) {
    return fileHandler->readStockData(symbol);
}

std::vector<double> StockPredictor::predict(const std::string& symbol, const std::string& algorithm) {
    auto it = algorithms.find(algorithm);
    if (it == algorithms.end()) {
        throw std::runtime_error("Unknown algorithm: " + algorithm);
    }

    auto data = getHistoricalData(symbol);
    auto predictions = it->second->predict(data);
    fileHandler->writePredictions(symbol, predictions);
    return predictions;
}

std::vector<std::string> StockPredictor::getAvailableAlgorithms() const {
    std::vector<std::string> result;
    for (const auto& algo : algorithms) {
        result.push_back(algo.first);
    }
    return result;
}

std::string StockPredictor::getDataDirectory() const {
    return fileHandler->getDataDirectory();
}

void StockPredictor::registerAlgorithm(const std::string& name, 
                                     std::unique_ptr<PredictionAlgorithm> algorithm) {
    algorithms[name] = std::move(algorithm);
}