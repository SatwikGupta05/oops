#include "../include/StockPredictor.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <nlohmann/json.hpp>
#include <memory>
#include <string>

using json = nlohmann::json;

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
        server.listen(host.c_str(), port);
    }

private:
    void setupRoutes() {
        // POST /api/analyze - Upload CSV and get predictions
        server.Post("/api/analyze", [this](const httplib::Request& req, httplib::Response& res) {
            try {
                // Parse the multipart form data
                if (!req.has_file("csv_file")) {
                    throw std::runtime_error("No CSV file uploaded");
                }

                // Get the file content
                const auto& file = req.get_file_value("csv_file");
                
                // Parse request parameters
                auto params = json::parse(req.get_param_value("params"));
                if (!params.is_object()) {
                    throw std::runtime_error("params must be a JSON object");
                }

                if (!params.contains("algorithms") || !params["algorithms"].is_array()) {
                    throw std::runtime_error("params must include 'algorithms' array");
                }

                auto& algorithms = params["algorithms"];
                
                // Validate algorithms and their parameters
                for (const auto& algo : algorithms) {
                    if (!algo.is_object() || !algo.contains("name")) {
                        throw std::runtime_error("Each algorithm must be an object with at least a 'name' field");
                    }
                }

                // Create temporary file
                std::string tempFileName = "temp_" + std::to_string(std::time(nullptr)) + ".csv";
                std::string tempFilePath = std::filesystem::path(predictor->getDataDirectory()) / tempFileName;
                
                // Write uploaded content to temp file
                std::ofstream tempFile(tempFilePath);
                if (!tempFile) {
                    throw std::runtime_error("Failed to create temporary file");
                }
                tempFile.write(file.content.c_str(), file.content.size());
                tempFile.close();

                // Process the file and get predictions
                json response = {{"predictions", json::object()}};
                std::string symbol = "UPLOADED"; // Use this as identifier for the uploaded file

                response["validations"] = json::array();
                response["errors"] = json::array();

                for (const auto& algo : algorithms) {
                    try {
                        std::string algoName = algo["name"].get<std::string>();
                        
                        // Configure algorithm if parameters are provided
                        if (algo.contains("parameters")) {
                            predictor->configureAlgorithm(algoName, algo["parameters"]);
                        }
                        
                        // Add algorithm configuration to response
                        auto config = predictor->getAlgorithmParameters(algoName);
                        response["validations"].push_back({
                            {"algorithm", algoName},
                            {"parameters", config}
                        });
                        
                        // Get predictions
                        auto predictions = predictor->predict(symbol, algoName);
                        response["predictions"][algoName] = predictions;
                    }
                    catch (const std::exception& e) {
                        response["errors"].push_back({
                            {"algorithm", algo["name"]},
                            {"error", e.what()}
                        });
                    }
                }

                // Clean up temp file
                std::filesystem::remove(tempFilePath);

                res.set_content(response.dump(), "application/json");

            } catch (const std::exception& e) {
                res.status = 400;
                json error = {{"error", e.what()}};
                res.set_content(error.dump(), "application/json");
            }
        });

        // GET /api/stocks/:symbol
        server.Get(R"(/api/stocks/([^/]+))", [this](const httplib::Request& req, httplib::Response& res) {
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

        // GET /api/algorithms
        server.Get("/api/algorithms", [this](const httplib::Request&, httplib::Response& res) {
            auto algorithms = predictor->getAvailableAlgorithms();
            json response = algorithms;
            res.set_content(response.dump(), "application/json");
        });

        // CORS support
        server.Options(".*", [](const httplib::Request&, httplib::Response& res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");
        });
    }
};