#include "../include/StockPredictor.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <fstream>
#include <ctime>

using json = nlohmann::json;

// Include StockServer class definition
class StockServer {
private:
    httplib::Server server;
    std::unique_ptr<StockPredictor> predictor;

public:
    StockServer(const std::string& dataDir) 
        : predictor(std::make_unique<StockPredictor>(dataDir)) {
        setupRoutes();
    }

    void start(const std::string& host, int port) {
        std::cout << "Server listening on http://" << host << ":" << port << std::endl;
        std::cout << "Available endpoints:" << std::endl;
        std::cout << "  GET  /" << std::endl;
        std::cout << "  GET  /api/stocks/{symbol}" << std::endl;
        std::cout << "  POST /api/predict" << std::endl;
        std::cout << "  POST /api/analyze" << std::endl;
        std::cout << "  GET  /api/algorithms" << std::endl;
        
        if (!server.listen(host.c_str(), port)) {
            throw std::runtime_error("Failed to start server on port " + std::to_string(port));
        }
    }

private:
    void setupRoutes() {
        // Root endpoint - health check
        server.Get("/", [](const httplib::Request&, httplib::Response& res) {
            json response = {
                {"status", "running"},
                {"message", "Stock Prediction API"},
                {"version", "1.0.0"},
                {"endpoints", {
                    {{"method", "GET"}, {"path", "/"}, {"description", "Health check"}},
                    {{"method", "GET"}, {"path", "/api/stocks/{symbol}"}, {"description", "Get historical stock data"}},
                    {{"method", "POST"}, {"path", "/api/predict"}, {"description", "Get stock predictions"}},
                    {{"method", "POST"}, {"path", "/api/analyze"}, {"description", "Upload CSV file and get predictions"}},
                    {{"method", "GET"}, {"path", "/api/algorithms"}, {"description", "List available algorithms"}}
                }}
            };
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(response.dump(2), "application/json");
        });

        // GET /api/stocks/:symbol
        server.Get(R"(/api/stocks/([^/]+))", [this](const httplib::Request& req, httplib::Response& res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            auto symbol = req.matches[1].str();
            try {
                auto data = predictor->getHistoricalData(symbol);
                json response = json::array();
                for (const auto& stock : data) {
                    json stockJson = {
                        {"symbol", stock.getSymbol()},
                        {"date", stock.getDate()},
                        {"open", stock.getOpen()},
                        {"high", stock.getHigh()},
                        {"low", stock.getLow()},
                        {"close", stock.getClose()},
                        {"volume", stock.getVolume()}
                    };
                    response.push_back(stockJson);
                }
                res.set_content(response.dump(), "application/json");
            } catch (const std::exception& e) {
                res.status = 404;
                json error = {{"error", e.what()}};
                res.set_content(error.dump(), "application/json");
            }
        });

        // POST /api/predict
        server.Post("/api/predict", [this](const httplib::Request& req, httplib::Response& res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            try {
                json body = json::parse(req.body);
                auto symbol = body["symbol"].get<std::string>();
                auto algorithm = body["algorithm"].get<std::string>();
                
                auto predictions = predictor->predict(symbol, algorithm);
                
                json response = {
                    {"symbol", symbol},
                    {"algorithm", algorithm},
                    {"predictions", predictions}
                };
                
                res.set_content(response.dump(), "application/json");
            } catch (const std::exception& e) {
                res.status = 400;
                json error = {{"error", e.what()}};
                res.set_content(error.dump(), "application/json");
            }
        });

        // POST /api/analyze - Upload CSV and get predictions
        server.Post("/api/analyze", [this](const httplib::Request& req, httplib::Response& res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            
            // Debug logging
            std::cout << "\n=== Received POST /api/analyze ===" << std::endl;
            std::cout << "Content-Type: " << req.get_header_value("Content-Type") << std::endl;
            std::cout << "Body size: " << req.body.size() << " bytes" << std::endl;
            std::cout << "Has csv_file param: " << (req.has_param("csv_file") ? "yes" : "no") << std::endl;
            std::cout << "Has csv_file file: " << (req.has_file("csv_file") ? "yes" : "no") << std::endl;
            std::cout << "Number of files: " << req.files.size() << std::endl;
            std::cout << "Number of params: " << req.params.size() << std::endl;
            
            // Print all file keys
            for (const auto& f : req.files) {
                std::cout << "File key: " << f.first << " (name: " << f.second.filename << ")" << std::endl;
            }
            
            try {
                // Check if file was uploaded
                if (!req.has_file("csv_file")) {
                    std::cout << "ERROR: No csv_file found in request!" << std::endl;
                    throw std::runtime_error("No CSV file uploaded");
                }

                // Get the uploaded file
                const auto& file = req.get_file_value("csv_file");
                
                // Parse optional algorithm and limit from form fields
                // In multipart/form-data, regular fields might be in params or files
                std::string algorithm = "";
                if (req.has_param("algorithm")) {
                    algorithm = req.get_param_value("algorithm");
                } else if (req.has_file("algorithm")) {
                    // Sometimes form fields come as files in multipart
                    algorithm = req.get_file_value("algorithm").content;
                }
                
                int limit = 10;
                std::string limitStr = "";
                if (req.has_param("limit")) {
                    limitStr = req.get_param_value("limit");
                } else if (req.has_file("limit")) {
                    // Sometimes form fields come as files in multipart
                    limitStr = req.get_file_value("limit").content;
                }
                
                if (!limitStr.empty()) {
                    try {
                        limit = std::stoi(limitStr);
                        // Validate limit
                        if (limit < 1 || limit > 100) {
                            limit = 10; // Reset to default if out of range
                        }
                    } catch (const std::exception&) {
                        limit = 10; // Use default if parsing fails
                    }
                }

                // Determine which algorithms to use
                std::vector<std::string> algorithmsToUse;
                if (!algorithm.empty()) {
                    algorithmsToUse.push_back(algorithm);
                } else {
                    // Use all available algorithms if none specified
                    auto availableAlgos = predictor->getAvailableAlgorithms();
                    for (const auto& algo : availableAlgos) {
                        algorithmsToUse.push_back(algo);
                    }
                }

                // Create temporary file with unique name
                std::string tempFileName = "temp_" + std::to_string(std::time(nullptr)) + ".csv";
                std::filesystem::path dataDir = std::filesystem::current_path() / "data";
                std::string tempFilePath = (dataDir / tempFileName).string();
                
                // Write uploaded content to temp file
                std::ofstream tempFile(tempFilePath);
                if (!tempFile) {
                    throw std::runtime_error("Failed to create temporary file");
                }
                tempFile.write(file.content.c_str(), file.content.size());
                tempFile.close();

                // Process the file and get predictions
                std::string symbol = tempFileName.substr(0, tempFileName.find_last_of('.')); // Remove .csv extension
                
                json response = {
                    {"predictions", json::object()},
                    {"validations", json::array()},
                    {"errors", json::array()},
                    {"metadata", {
                        {"limit", limit},
                        {"algorithms_requested", algorithm.empty() ? "all" : algorithm},
                        {"file_name", file.filename}
                    }}
                };

                for (const auto& algoName : algorithmsToUse) {
                    try {
                        // Get predictions
                        auto predictions = predictor->predict(symbol, algoName);
                        
                        // Limit the predictions to the requested number
                        json limitedPredictions = json::array();
                        int count = 0;
                        for (const auto& pred : predictions) {
                            if (count >= limit) break;
                            limitedPredictions.push_back(pred);
                            count++;
                        }
                        
                        response["predictions"][algoName] = limitedPredictions;
                        
                        // Add algorithm parameters info
                        if (algoName == "SMA") {
                            response["validations"].push_back({
                                {"algorithm", algoName},
                                {"parameters", {{"window_size", 5}, {"min_window", 2}, {"max_window", 200}}},
                                {"prediction_count", count}
                            });
                        } else if (algoName == "EMA") {
                            response["validations"].push_back({
                                {"algorithm", algoName},
                                {"parameters", {{"alpha", 0.2}, {"min_alpha", 0.0001}, {"max_alpha", 1.0}}},
                                {"prediction_count", count}
                            });
                        } else {
                            response["validations"].push_back({
                                {"algorithm", algoName},
                                {"parameters", "default"},
                                {"prediction_count", count}
                            });
                        }
                    }
                    catch (const std::exception& e) {
                        response["errors"].push_back({
                            {"algorithm", algoName},
                            {"error", e.what()}
                        });
                    }
                }

                // Clean up temp file
                std::filesystem::remove(tempFilePath);

                res.set_content(response.dump(2), "application/json");

            } catch (const std::exception& e) {
                res.status = 400;
                json error = {{"error", e.what()}};
                res.set_content(error.dump(), "application/json");
            }
        });

        // GET /api/algorithms
        server.Get("/api/algorithms", [this](const httplib::Request&, httplib::Response& res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            auto algorithms = predictor->getAvailableAlgorithms();
            json response = algorithms;
            res.set_content(response.dump(), "application/json");
        });

        // POST /api/test - Simple test endpoint for debugging
        server.Post("/api/test", [](const httplib::Request& req, httplib::Response& res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            std::cout << "\n=== Test POST received ===" << std::endl;
            std::cout << "Body: " << req.body << std::endl;
            
            json response = {
                {"status", "success"},
                {"message", "POST request received successfully"},
                {"body_length", req.body.size()},
                {"has_files", req.files.size() > 0}
            };
            res.set_content(response.dump(2), "application/json");
        });

        // CORS support
        server.Options(".*", [](const httplib::Request&, httplib::Response& res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");
        });
    }
};

int main() {
    try {
        // Get port from environment variable or use default
        int port = 3000;
        if (const char* env_port = std::getenv("PORT")) {
            port = std::stoi(env_port);
        }

        // Set up data directory
        std::filesystem::path dataDir = std::filesystem::current_path() / "data";
        if (!std::filesystem::exists(dataDir)) {
            std::filesystem::create_directory(dataDir);
        }

        // Create and start server
        StockServer server(dataDir.string());
        std::cout << "Starting server on port " << port << std::endl;
        server.start("0.0.0.0", port);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}