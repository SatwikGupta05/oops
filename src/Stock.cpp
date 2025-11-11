#include "../include/Stock.h"
#include <iostream>

StockData::StockData(const std::string& symbol, const std::string& date,
                     double open, double high, double low, double close, double volume)
    : symbol(symbol), date(date), open(open), high(high), low(low), close(close), volume(volume) {}

void StockData::display() const {
    std::cout << "Symbol: " << symbol << "\n"
              << "Date: " << date << "\n"
              << "Open: " << open << "\n"
              << "High: " << high << "\n"
              << "Low: " << low << "\n"
              << "Close: " << close << "\n"
              << "Volume: " << volume << "\n";
}