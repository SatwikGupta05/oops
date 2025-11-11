# Stock Prediction Backend

A high-performance C++ REST API server for stock data management and price prediction using various algorithms.

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
- [API Documentation](#api-documentation)
- [Docker Support](#docker-support)
- [Project Structure](#project-structure)
- [Technologies Used](#technologies-used)
- [Contributing](#contributing)

## 🔍 Overview

This project provides a RESTful API server built in C++ for managing historical stock data and generating price predictions using multiple algorithms. The server offers endpoints for retrieving stock information, running predictions, and listing available prediction algorithms.

## ✨ Features

- **Historical Data Retrieval**: Fetch historical stock data for any symbol
- **CSV File Upload & Analysis**: Upload custom CSV files and get instant predictions
- **Multiple Prediction Algorithms**:
  - Simple Moving Average (SMA) with configurable window size (2-200 days)
  - Exponential Moving Average (EMA) with configurable smoothing factor (0.0001-1.0)
- **Algorithm Configuration**: Dynamic configuration of algorithm parameters
- **Batch Predictions**: Run multiple algorithms simultaneously on uploaded data
- **RESTful API**: Clean and intuitive REST endpoints
- **CORS Support**: Cross-Origin Resource Sharing enabled for cross-origin requests
- **Docker Support**: Fully containerized deployment with Docker and Docker Compose
- **JSON Responses**: All responses in JSON format
- **Error Handling**: Comprehensive error handling with detailed error messages
- **Data Persistence**: Automatic saving of predictions to CSV files
- **Health Check Endpoint**: Monitor server status and available endpoints

## 🏗️ Architecture

The project follows a modular architecture with clear separation of concerns:

- **HTTP Server Layer**: Handles incoming requests and routing
- **Business Logic Layer**: Stock prediction and data management
- **Data Access Layer**: File-based storage for stock data
- **Algorithm Layer**: Pluggable prediction algorithms

### Key Components

- **StockServer**: HTTP server implementation with route definitions
- **StockPredictor**: Main business logic for predictions
- **FileHandler**: Manages reading/writing stock data from CSV files
- **PredictionAlgorithm**: Base class for prediction algorithms
- **Stock**: Data model representing stock information

## 📦 Prerequisites

Before building the project, ensure you have:

- **CMake** 3.10 or higher
- **C++17** compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **OpenSSL** development libraries
- **Git** (for fetching dependencies)

### Installing Prerequisites

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential cmake git libssl-dev
```

#### macOS
```bash
brew install cmake openssl
```

#### Windows
- Install [CMake](https://cmake.org/download/)
- Install [Visual Studio](https://visualstudio.microsoft.com/) with C++ tools
- Install [OpenSSL](https://slproweb.com/products/Win32OpenSSL.html)

## 🚀 Installation

### 1. Clone the Repository

```bash
git clone <repository-url>
cd stock-backend
```

### 2. Build the Project

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### 3. Prepare Data

Ensure you have CSV files in the `data/` directory. Files should be named `{SYMBOL}.csv`:

```csv
Date,Open,High,Low,Close,Volume
2025-11-01,150.0,155.0,149.0,153.0,1000000
2025-11-02,153.0,157.0,152.0,156.0,1200000
```

**Note**: Each stock symbol needs its own CSV file (e.g., `AAPL.csv`, `MSFT.csv`).

## 💻 Usage

### Running the Server

```bash
# From the build directory
./stock_server
```

The server will start on `http://0.0.0.0:3000` by default.

### Custom Port

Set the `PORT` environment variable:

```bash
# Linux/macOS
export PORT=8080
./stock_server

# Windows (cmd)
set PORT=8080
stock_server.exe

# Windows (PowerShell)
$env:PORT=8080
.\stock_server.exe
```

### Server Output

When the server starts, you'll see:

```
Starting server on port 3000
Server listening on http://0.0.0.0:3000
        std::cout << "Available endpoints:" << std::endl;
        std::cout << "  GET  /" << std::endl;
        std::cout << "  GET  /api/stocks/{symbol}" << std::endl;
        std::cout << "  POST /api/predict" << std::endl;
        std::cout << "  POST /api/analyze" << std::endl;
        std::cout << "  GET  /api/algorithms" << std::endl;
```

## 📚 API Documentation

For detailed API documentation including request/response examples, see [API.md](API.md).

Quick reference:

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/` | Health check |
| GET | `/api/stocks/{symbol}` | Get historical data |
| POST | `/api/predict` | Get predictions |
| POST | `/api/analyze` | Upload CSV & get predictions |
| GET | `/api/algorithms` | List algorithms |

## 🐳 Docker Support

### Quick Start with Docker Compose (Recommended)

```bash
# Build and start the container
docker-compose up --build

# Or run in detached mode
docker-compose up -d

# View logs
docker-compose logs -f

# Stop the container
docker-compose down
```

### Building the Docker Image Manually

```bash
docker build -t stock-backend .
```

### Running with Docker

```bash
# Run the container
docker run -p 3000:3000 -v $(pwd)/data:/app/data stock-backend

# Run in background
docker run -d -p 3000:3000 -v $(pwd)/data:/app/data stock-backend
```

### Docker Configuration

The Docker setup includes:
- **Base Image**: Ubuntu 22.04
- **Build Tools**: CMake, GCC, Git, OpenSSL
- **External Dependencies**: cpp-httplib, nlohmann-json (fetched automatically)
- **Port Mapping**: 3000:3000
- **Volume Mount**: `./data:/app/data` for persistent data storage
- **Environment**: PORT=3000 (configurable)

The service will be available at `http://localhost:3000`.

### Testing the Dockerized Application

```powershell
# Health check
Invoke-RestMethod -Uri http://localhost:3000/

# Get algorithms
Invoke-RestMethod -Uri http://localhost:3000/api/algorithms

# Get stock data
Invoke-RestMethod -Uri http://localhost:3000/api/stocks/AAPL

# Get predictions
Invoke-RestMethod -Uri http://localhost:3000/api/predict -Method POST -ContentType "application/json" -Body '{"symbol":"AAPL","algorithm":"SMA"}'
```

## 📁 Project Structure

```
stock-backend/
├── CMakeLists.txt          # CMake build configuration
├── Dockerfile              # Docker container definition
├── docker-compose.yml      # Docker Compose configuration
├── .dockerignore           # Docker build exclusions
├── README.md               # This file
├── API.md                  # API documentation
├── data/                   # Stock data CSV files
│   ├── AAPL.csv            # Apple stock data
│   ├── MSFT.csv            # Microsoft stock data
│   ├── AAPL_predictions.csv # Generated predictions for AAPL
│   └── MSFT_predictions.csv # Generated predictions for MSFT
├── include/                # Header files
│   ├── FileHandler.h       # File I/O operations
│   ├── PredictionAlgorithm.h # Algorithm base class and implementations
│   ├── Stock.h             # Stock data model
│   └── StockPredictor.h    # Main prediction orchestrator
└── src/                    # Source files
    ├── FileHandler.cpp     # File operations implementation
    ├── main.cpp            # HTTP server and API endpoints
    ├── PredictionAlgorithm.cpp # Algorithm implementations
    ├── Stock.cpp           # Stock data model implementation
    └── StockPredictor.cpp  # Prediction logic implementation
```

## 🛠️ Technologies Used

- **[cpp-httplib](https://github.com/yhirose/cpp-httplib)**: HTTP server library (v0.11.3)
- **[nlohmann/json](https://github.com/nlohmann/json)**: JSON parsing library (v3.11.2)
- **OpenSSL**: SSL/TLS support
- **CMake**: Build system
- **Docker**: Containerization

## 🔧 Development

### Adding New Prediction Algorithms

1. Create a new class inheriting from `PredictionAlgorithm`
2. Implement the required virtual methods:
   - `predict()` - Core prediction logic
   - `getName()` - Algorithm identifier
   - `getDescription()` - Human-readable description
   - `configure()` - Handle configuration parameters
   - `getParameters()` - Return current parameters
   - `validate()` - Validate parameters
3. Register the algorithm in `StockPredictor::initializeAlgorithms()`

Example:

```cpp
class MyAlgorithm : public PredictionAlgorithm {
private:
    int windowSize;
    static constexpr int MIN_WINDOW = 2;
    static constexpr int MAX_WINDOW = 50;

public:
    explicit MyAlgorithm(int window = 10) : windowSize(window) {
        validate();
    }
    
    std::vector<double> predict(const std::vector<StockData>& data) override {
        // Your implementation
    }
    
    std::string getName() const override { return "MyAlgo"; }
    std::string getDescription() const override { return "My Custom Algorithm"; }
    
    void configure(const nlohmann::json& params) override {
        if (params.contains("window")) {
            windowSize = params["window"].get<int>();
        }
        validate();
    }
    
    nlohmann::json getParameters() const override {
        return {{"window", windowSize}, {"min", MIN_WINDOW}, {"max", MAX_WINDOW}};
    }
    
    void validate() const override {
        if (windowSize < MIN_WINDOW || windowSize > MAX_WINDOW) {
            throw std::invalid_argument("Window size out of range");
        }
    }
};
```

### Adding New Routes

Add new routes in the `setupRoutes()` method in `main.cpp`:

```cpp
server.Get("/api/new-route", [this](const httplib::Request& req, httplib::Response& res) {
    // Handler implementation
});
```

## 🤝 Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is open-source and available under the MIT License.

## 📧 Contact

For questions or support, please open an issue in the repository.

## 🐛 Troubleshooting

### Docker Build Issues

If you encounter build errors:

1. **Missing nlohmann/json include**: Ensure `#include <nlohmann/json.hpp>` is in `PredictionAlgorithm.h`
2. **OpenSSL not found**: Install `libssl-dev` package
3. **CMake version**: Ensure CMake 3.10 or higher is installed

### Runtime Issues

1. **Port already in use**: Change the port using `PORT` environment variable
2. **Data not found**: Ensure CSV files are in the `data/` directory with correct naming (`{SYMBOL}.csv`)
3. **Prediction failures**: Check that CSV files have at least 5 data points for SMA

### Docker Container Issues

```bash
# Check container status
docker ps -a

# View container logs
docker-compose logs -f

# Restart container
docker-compose restart

# Rebuild from scratch
docker-compose down
docker-compose up --build
```

## 📊 Prediction Output

When you call the `/api/predict` endpoint, the server:
1. Reads historical data from `data/{SYMBOL}.csv`
2. Applies the selected algorithm (SMA or EMA)
3. Generates predictions
4. Saves predictions to `data/{SYMBOL}_predictions.csv`
5. Returns predictions in the API response

**Prediction CSV Format**:
```csv
Date,Predicted_Close
YYYY-MM-DD,153.82
YYYY-MM-DD,154.83
YYYY-MM-DD,155.75
```

**Note**: Dates are currently placeholders. Future versions will include actual future dates.

---

## ⚡ Performance

- **Response Time**: < 50ms for typical requests
- **Prediction Speed**: Depends on data size and algorithm
  - SMA: O(n × w) where n = data points, w = window size
  - EMA: O(n) where n = data points
- **Concurrent Requests**: Supported via cpp-httplib multi-threading

---

**Note**: This is a development server. For production use, consider adding:
- Authentication and authorization
- Rate limiting
- Input validation and sanitization
- HTTPS/TLS support
- Logging and monitoring
- Database backend (instead of CSV files)
- Caching layer
