#pragma once
#include <string>
#include <vector>

class StockData {
private:
    std::string symbol;
    std::string date;
    double open;
    double high;
    double low;
    double close;
    double volume;

public:
    // Constructor
    StockData(const std::string& symbol, const std::string& date,
              double open, double high, double low, double close, double volume);

    // Getters
    std::string getSymbol() const { return symbol; }
    std::string getDate() const { return date; }
    double getOpen() const { return open; }
    double getHigh() const { return high; }
    double getLow() const { return low; }
    double getClose() const { return close; }
    double getVolume() const { return volume; }

    // Display method
    void display() const;
};