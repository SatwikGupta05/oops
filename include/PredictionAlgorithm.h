#pragma once
#include "Stock.h"
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

class PredictionAlgorithm {
public:
    virtual ~PredictionAlgorithm() = default;
    
    // Pure virtual functions
    virtual std::vector<double> predict(const std::vector<StockData>& data) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Configuration
    virtual void configure(const nlohmann::json& params) = 0;
    virtual nlohmann::json getParameters() const = 0;
    virtual void validate() const = 0;

protected:
    // Utility functions for derived classes
    std::vector<double> getClosingPrices(const std::vector<StockData>& data);
};

// Concrete implementations
class MovingAverageAlgorithm : public PredictionAlgorithm {
private:
    int windowSize;
    static constexpr int MIN_WINDOW = 2;
    static constexpr int MAX_WINDOW = 200;

public:
    explicit MovingAverageAlgorithm(int window = 5);
    std::vector<double> predict(const std::vector<StockData>& data) override;
    std::string getName() const override { return "SMA"; }
    std::string getDescription() const override;
    
    void configure(const nlohmann::json& params) override;
    nlohmann::json getParameters() const override;
    void validate() const override;
};

class ExponentialMovingAverageAlgorithm : public PredictionAlgorithm {
private:
    double smoothingFactor;
    static constexpr double MIN_ALPHA = 0.0001;
    static constexpr double MAX_ALPHA = 1.0;

public:
    explicit ExponentialMovingAverageAlgorithm(double alpha = 0.2);
    std::vector<double> predict(const std::vector<StockData>& data) override;
    std::string getName() const override { return "EMA"; }
    std::string getDescription() const override;
    
    void configure(const nlohmann::json& params) override;
    nlohmann::json getParameters() const override;
    void validate() const override;
};