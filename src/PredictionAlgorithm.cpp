#include "../include/PredictionAlgorithm.h"
#include <numeric>
#include <stdexcept>
#include <sstream>

std::vector<double> PredictionAlgorithm::getClosingPrices(const std::vector<StockData>& data) {
    std::vector<double> prices;
    prices.reserve(data.size());
    for (const auto& stock : data) {
        prices.push_back(stock.getClose());
    }
    return prices;
}

// Moving Average Implementation
MovingAverageAlgorithm::MovingAverageAlgorithm(int window) : windowSize(window) {
    validate();
}

void MovingAverageAlgorithm::configure(const nlohmann::json& params) {
    if (params.contains("window_size")) {
        windowSize = params["window_size"].get<int>();
    }
    validate();
}

nlohmann::json MovingAverageAlgorithm::getParameters() const {
    return {
        {"window_size", windowSize},
        {"min_window", MIN_WINDOW},
        {"max_window", MAX_WINDOW}
    };
}

void MovingAverageAlgorithm::validate() const {
    if (windowSize < MIN_WINDOW || windowSize > MAX_WINDOW) {
        std::stringstream ss;
        ss << "Window size must be between " << MIN_WINDOW << " and " << MAX_WINDOW;
        throw std::invalid_argument(ss.str());
    }
}

std::vector<double> MovingAverageAlgorithm::predict(const std::vector<StockData>& data) {
    std::vector<double> prices = getClosingPrices(data);
    std::vector<double> predictions;
    
    if (prices.size() < static_cast<size_t>(windowSize)) {
        throw std::runtime_error("Insufficient data points for the specified window size");
    }

    predictions.reserve(prices.size() - windowSize + 1);
    for (size_t i = windowSize; i <= prices.size(); ++i) {
        double sum = std::accumulate(prices.begin() + (i - windowSize), prices.begin() + i, 0.0);
        predictions.push_back(sum / windowSize);
    }

    return predictions;
}

std::string MovingAverageAlgorithm::getDescription() const {
    return "Simple Moving Average (SMA) using " + std::to_string(windowSize) + " day window";
}

// Exponential Moving Average Implementation
ExponentialMovingAverageAlgorithm::ExponentialMovingAverageAlgorithm(double alpha) 
    : smoothingFactor(alpha) {
    validate();
}

void ExponentialMovingAverageAlgorithm::configure(const nlohmann::json& params) {
    if (params.contains("alpha")) {
        smoothingFactor = params["alpha"].get<double>();
    }
    validate();
}

nlohmann::json ExponentialMovingAverageAlgorithm::getParameters() const {
    return {
        {"alpha", smoothingFactor},
        {"min_alpha", MIN_ALPHA},
        {"max_alpha", MAX_ALPHA}
    };
}

void ExponentialMovingAverageAlgorithm::validate() const {
    if (smoothingFactor < MIN_ALPHA || smoothingFactor > MAX_ALPHA) {
        std::stringstream ss;
        ss << "Smoothing factor (alpha) must be between " << MIN_ALPHA << " and " << MAX_ALPHA;
        throw std::invalid_argument(ss.str());
    }
}

std::vector<double> ExponentialMovingAverageAlgorithm::predict(const std::vector<StockData>& data) {
    std::vector<double> prices = getClosingPrices(data);
    std::vector<double> predictions;

    if (prices.empty()) {
        throw std::runtime_error("No data points provided for prediction");
    }

    predictions.reserve(prices.size());
    double ema = prices[0];
    predictions.push_back(ema);

    for (size_t i = 1; i < prices.size(); ++i) {
        ema = smoothingFactor * prices[i] + (1 - smoothingFactor) * ema;
        predictions.push_back(ema);
    }

    return predictions;
}

std::string ExponentialMovingAverageAlgorithm::getDescription() const {
    return "Exponential Moving Average (EMA) with smoothing factor " + std::to_string(smoothingFactor);
}