# Stock Prediction API Documentation

Complete API reference for the Stock Prediction Backend server.

## Base URL

```
http://localhost:3000
```

## Common Headers

All endpoints support CORS and accept/return JSON:

```
Content-Type: application/json
Access-Control-Allow-Origin: *
```

---

## Endpoints

### 1. Health Check

**Description**: Check if the API server is running and get basic information about available endpoints.

**Endpoint**: `GET /`

**Request**: No parameters required

**Response**: 

```json
{
  "status": "running",
  "message": "Stock Prediction API",
  "version": "1.0.0",
  "endpoints": [
    {
      "method": "GET",
      "path": "/",
      "description": "Health check"
    },
    {
      "method": "GET",
      "path": "/api/stocks/{symbol}",
      "description": "Get historical stock data"
    },
    {
      "method": "POST",
      "path": "/api/predict",
      "description": "Get stock predictions"
    },
    {
      "method": "POST",
      "path": "/api/analyze",
      "description": "Upload CSV file and get predictions"
    },
    {
      "method": "GET",
      "path": "/api/algorithms",
      "description": "List available algorithms"
    }
  ]
}
```

**Status Codes**:
- `200 OK`: Server is running

**Example**:

```bash
curl http://localhost:3000/
```

---

### 2. Get Historical Stock Data

**Description**: Retrieve historical stock data for a specific stock symbol.

**Endpoint**: `GET /api/stocks/{symbol}`

**URL Parameters**:
- `symbol` (required): Stock symbol (e.g., AAPL, MSFT, GOOGL)

**Request**: No body required

**Response**: 

```json
[
  {
    "symbol": "AAPL",
    "date": "2024-01-01",
    "open": 150.0,
    "high": 155.0,
    "low": 149.0,
    "close": 153.0,
    "volume": 1000000
  },
  {
    "symbol": "AAPL",
    "date": "2024-01-02",
    "open": 153.0,
    "high": 157.0,
    "low": 152.0,
    "close": 156.0,
    "volume": 1200000
  }
]
```

**Response Fields**:
- `symbol` (string): Stock symbol
- `date` (string): Date in YYYY-MM-DD format
- `open` (number): Opening price
- `high` (number): Highest price of the day
- `low` (number): Lowest price of the day
- `close` (number): Closing price
- `volume` (number): Trading volume

**Status Codes**:
- `200 OK`: Data retrieved successfully
- `404 Not Found`: Stock symbol not found

**Error Response**:

```json
{
  "error": "Stock data not found for symbol: INVALID"
}
```

**Examples**:

```bash
# Get AAPL historical data
curl http://localhost:3000/api/stocks/AAPL

# Get MSFT historical data
curl http://localhost:3000/api/stocks/MSFT
```

**JavaScript Example**:

```javascript
fetch('http://localhost:3000/api/stocks/AAPL')
  .then(response => response.json())
  .then(data => console.log(data))
  .catch(error => console.error('Error:', error));
```

---

### 3. Get Stock Predictions

**Description**: Generate price predictions for a stock using a specified algorithm.

**Endpoint**: `POST /api/predict`

**Request Body**:

```json
{
  "symbol": "AAPL",
  "algorithm": "SMA"
}
```

**Request Fields**:
- `symbol` (string, required): Stock symbol to predict
- `algorithm` (string, required): Algorithm to use (see available algorithms)
  - `"SMA"` - Simple Moving Average
  - `"EMA"` - Exponential Moving Average

**Response**: 

```json
{
  "symbol": "AAPL",
  "algorithm": "SMA",
  "predictions": [
    152.5,
    153.2,
    154.0,
    155.1,
    156.3
  ]
}
```

**Response Fields**:
- `symbol` (string): Stock symbol that was predicted
- `algorithm` (string): Algorithm used for prediction
- `predictions` (array of numbers): Predicted prices
  - SMA: Returns 6 predictions (for 10 data points with window size 5)
  - EMA: Returns 10 predictions (one for each data point)

**Side Effects**:
- Predictions are automatically saved to `data/{SYMBOL}_predictions.csv`

**Status Codes**:
- `200 OK`: Predictions generated successfully
- `400 Bad Request`: Invalid request body or parameters

**Error Response**:

```json
{
  "error": "Invalid algorithm: UNKNOWN"
}
```

**Examples**:

```bash
# Simple Moving Average prediction
curl -X POST http://localhost:3000/api/predict \
  -H "Content-Type: application/json" \
  -d '{"symbol": "AAPL", "algorithm": "SMA"}'

# Exponential Moving Average prediction
curl -X POST http://localhost:3000/api/predict \
  -H "Content-Type: application/json" \
  -d '{"symbol": "MSFT", "algorithm": "EMA"}'
```

**JavaScript Example**:

```javascript
fetch('http://localhost:3000/api/predict', {
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
  },
  body: JSON.stringify({
    symbol: 'AAPL',
    algorithm: 'SMA'
  })
})
  .then(response => response.json())
  .then(data => console.log(data))
  .catch(error => console.error('Error:', error));
```

**Python Example**:

```python
import requests

url = "http://localhost:3000/api/predict"
payload = {
    "symbol": "AAPL",
    "algorithm": "SMA"
}

response = requests.post(url, json=payload)
print(response.json())
```

---

### 4. Upload CSV and Analyze

**Description**: Upload a custom CSV file and get predictions from one or all available algorithms. This endpoint accepts multipart form data with a CSV file and optional configuration parameters.

**Endpoint**: `POST /api/analyze`

**Request Type**: `multipart/form-data`

**Request Parameters**:

1. **csv_file** (file, required): CSV file containing stock data
   - Must follow the standard format: Date,Open,High,Low,Close,Volume
   - Minimum 5 data points recommended for SMA

2. **algorithm** (text, optional): Specific algorithm to use
   - Valid values: "SMA", "EMA"
   - If omitted: Runs all available algorithms

3. **limit** (text, optional): Number of predictions to return
   - Must be an integer between 1 and 100
   - Default: 10
   - If out of range: Resets to default (10)

**Response**: 

```json
{
  "predictions": {
    "SMA": [152.5, 153.2, 154.0, 155.1, 156.3],
    "EMA": [150.0, 150.2, 150.56, 150.848, 151.478]
  },
  "validations": [
    {
      "algorithm": "SMA",
      "parameters": {
        "window_size": 5,
        "min_window": 2,
        "max_window": 200
      },
      "prediction_count": 5
    },
    {
      "algorithm": "EMA",
      "parameters": {
        "alpha": 0.2,
        "min_alpha": 0.0001,
        "max_alpha": 1.0
      },
      "prediction_count": 5
    }
  ],
  "errors": [],
  "metadata": {
    "limit": 5,
    "algorithms_requested": "all",
    "file_name": "sample_stock.csv"
  }
}
```

**Response Fields**:
- `predictions` (object): Object containing predictions for each algorithm
  - Keys are algorithm names
  - Values are arrays of predicted prices (limited to requested limit)
- `validations` (array): Configuration validation for each algorithm
  - Shows the parameters used and their valid ranges
  - Includes `prediction_count`: actual number of predictions returned
- `errors` (array): Any errors encountered for specific algorithms
  - Contains algorithm name and error message
- `metadata` (object): Request metadata
  - `limit` (number): Number of predictions returned per algorithm
  - `algorithms_requested` (string): "all" or specific algorithm name
  - `file_name` (string): Original uploaded file name

**Status Codes**:
- `200 OK`: Analysis completed successfully
- `400 Bad Request`: Invalid request (missing file, malformed JSON, invalid parameters)

**Error Response**:

```json
{
  "error": "No CSV file uploaded"
}
```

Or with partial success (some algorithms failed):

```json
{
  "predictions": {
    "SMA": [152.5, 153.2, 154.0]
  },
  "validations": [
    {
      "algorithm": "SMA",
      "parameters": {
        "window_size": 5,
        "min_window": 2,
        "max_window": 200
      },
      "prediction_count": 3
    }
  ],
  "errors": [
    {
      "algorithm": "EMA",
      "error": "Failed to generate predictions"
    }
  ],
  "metadata": {
    "limit": 10,
    "algorithms_requested": "all",
    "file_name": "sample_stock.csv"
  }
}
```

**Examples**:

**cURL Example**:

```bash
# Create a sample CSV file
cat > sample_stock.csv << EOF
Date,Open,High,Low,Close,Volume
2025-11-01,150.0,155.0,149.0,153.0,1000000
2025-11-02,153.0,157.0,152.0,156.0,1200000
2025-11-03,156.0,159.0,155.0,158.0,1300000
2025-11-04,158.0,161.0,157.0,160.0,1400000
2025-11-05,160.0,163.0,159.0,162.0,1500000
EOF

# Upload file only (runs all algorithms with default limit of 10)
curl -X POST http://localhost:3000/api/analyze \
  -F "csv_file=@sample_stock.csv"

# Upload with specific algorithm
curl -X POST http://localhost:3000/api/analyze \
  -F "csv_file=@sample_stock.csv" \
  -F "algorithm=SMA"

# Upload with specific algorithm and limit
curl -X POST http://localhost:3000/api/analyze \
  -F "csv_file=@sample_stock.csv" \
  -F "algorithm=SMA" \
  -F "limit=5"

# Upload with custom limit for all algorithms
curl -X POST http://localhost:3000/api/analyze \
  -F "csv_file=@sample_stock.csv" \
  -F "limit=20"
```

**JavaScript Example (Browser)**:

```javascript
// HTML: <input type="file" id="csvFile" />

async function analyzeCsv() {
  const fileInput = document.getElementById('csvFile');
  const file = fileInput.files[0];
  
  if (!file) {
    alert('Please select a CSV file');
    return;
  }
  
  const formData = new FormData();
  formData.append('csv_file', file);
  
  // Optional: specify algorithm (omit to run all algorithms)
  formData.append('algorithm', 'SMA');
  
  // Optional: specify limit (omit to use default of 10)
  formData.append('limit', '15');
  
  try {
    const response = await fetch('http://localhost:3000/api/analyze', {
      method: 'POST',
      body: formData
    });
    
    const result = await response.json();
    console.log('Predictions:', result.predictions);
    console.log('Validations:', result.validations);
    console.log('Metadata:', result.metadata);
    
    if (result.errors.length > 0) {
      console.error('Some algorithms failed:', result.errors);
    }
  } catch (error) {
    console.error('Error:', error);
  }
}

// Example: Run all algorithms with default settings
async function analyzeCsvSimple() {
  const fileInput = document.getElementById('csvFile');
  const file = fileInput.files[0];
  
  const formData = new FormData();
  formData.append('csv_file', file);
  
  const response = await fetch('http://localhost:3000/api/analyze', {
    method: 'POST',
    body: formData
  });
  
  const result = await response.json();
  console.log('All predictions:', result.predictions);
}
```

**Python Example**:

```python
import requests

# Example 1: Upload CSV with all algorithms (default limit)
url = "http://localhost:3000/api/analyze"

with open('sample_stock.csv', 'rb') as f:
    files = {'csv_file': f}
    response = requests.post(url, files=files)
    result = response.json()
    
print('Predictions:', result['predictions'])
print('Metadata:', result['metadata'])

# Example 2: Upload CSV with specific algorithm and limit
with open('sample_stock.csv', 'rb') as f:
    files = {'csv_file': f}
    data = {
        'algorithm': 'SMA',
        'limit': '20'
    }
    response = requests.post(url, files=files, data=data)
    result = response.json()
    
print('SMA Predictions:', result['predictions']['SMA'])
print('Validations:', result['validations'])
if result['errors']:
    print('Errors:', result['errors'])

# Example 3: Upload CSV with custom limit for all algorithms
with open('sample_stock.csv', 'rb') as f:
    files = {'csv_file': f}
    data = {'limit': '5'}
    response = requests.post(url, files=files, data=data)
    result = response.json()
    
for algo, predictions in result['predictions'].items():
    print(f'{algo}: {predictions}')
```

**PowerShell Example**:

```powershell
# Create sample CSV
$csv = @"
Date,Open,High,Low,Close,Volume
2025-11-01,150.0,155.0,149.0,153.0,1000000
2025-11-02,153.0,157.0,152.0,156.0,1200000
2025-11-03,156.0,159.0,155.0,158.0,1300000
2025-11-04,158.0,161.0,157.0,160.0,1400000
2025-11-05,160.0,163.0,159.0,162.0,1500000
"@
$csv | Out-File -FilePath "sample_stock.csv" -Encoding ASCII

# Example 1: Upload file only (all algorithms, default limit)
$response = Invoke-RestMethod -Uri "http://localhost:3000/api/analyze" `
    -Method POST `
    -Form @{
        csv_file = Get-Item -Path "sample_stock.csv"
    }

Write-Host "All Predictions:"
$response.predictions | ConvertTo-Json
Write-Host "`nMetadata:"
$response.metadata | ConvertTo-Json

# Example 2: Upload with specific algorithm
$response = Invoke-RestMethod -Uri "http://localhost:3000/api/analyze" `
    -Method POST `
    -Form @{
        csv_file = Get-Item -Path "sample_stock.csv"
        algorithm = "SMA"
    }

Write-Host "`nSMA Predictions:"
$response.predictions.SMA | ConvertTo-Json

# Example 3: Upload with algorithm and limit
$response = Invoke-RestMethod -Uri "http://localhost:3000/api/analyze" `
    -Method POST `
    -Form @{
        csv_file = Get-Item -Path "sample_stock.csv"
        algorithm = "EMA"
        limit = "5"
    }

Write-Host "`nEMA Predictions (limited to 5):"
$response.predictions.EMA | ConvertTo-Json
Write-Host "`nValidations:"
$response.validations | ConvertTo-Json

# Example 4: Upload with custom limit for all algorithms
$response = Invoke-RestMethod -Uri "http://localhost:3000/api/analyze" `
    -Method POST `
    -Form @{
        csv_file = Get-Item -Path "sample_stock.csv"
        limit = "15"
    }

Write-Host "`nAll Predictions (limited to 15 each):"
$response.predictions | ConvertTo-Json -Depth 3
```

**Use Cases**:

1. **Quick Analysis**: Upload historical data without storing it on the server
2. **Algorithm Comparison**: Test all available algorithms on the same dataset
3. **Custom Data**: Analyze stock data from custom sources
4. **Batch Processing**: Process multiple files programmatically
5. **Flexible Testing**: Choose specific algorithms or run all with customizable result limits

**Notes**:

- The uploaded file is temporarily stored and deleted after processing
- Predictions are limited to the requested number (1-100, default 10)
- Predictions are not saved to CSV files (unlike `/api/predict`)
- All algorithms run independently - one failure doesn't affect others
- If algorithm parameter is omitted, all available algorithms are executed
- Returns metadata showing what was requested and actual results
- Algorithm parameters use default values (cannot be customized in this endpoint)

---

### 5. List Available Algorithms

**Description**: Get a list of all available prediction algorithms supported by the server.

**Endpoint**: `GET /api/algorithms`

**Request**: No parameters required

**Response**: 

```json
[
  "SMA",
  "EMA"
]
```

**Response**: Array of algorithm identifiers (strings)

**Algorithm Details**:
- **SMA** (Simple Moving Average): 
  - Uses a rolling average of closing prices
  - Window size: 5 days (default)
  - Configurable range: 2-200 days
  - Formula: `avg = (price[i-4] + price[i-3] + price[i-2] + price[i-1] + price[i]) / 5`
  - Output: `data_points - window_size + 1` predictions

- **EMA** (Exponential Moving Average): 
  - Weighted average giving more importance to recent prices
  - Smoothing factor (α): 0.2 (default)
  - Configurable range: 0.0001-1.0
  - Formula: `EMA[i] = α × price[i] + (1-α) × EMA[i-1]`
  - Output: Same number of predictions as input data points

**Status Codes**:
- `200 OK`: Algorithms retrieved successfully

**Example**:

```bash
curl http://localhost:3000/api/algorithms
```

**JavaScript Example**:

```javascript
fetch('http://localhost:3000/api/algorithms')
  .then(response => response.json())
  .then(algorithms => {
    console.log('Available algorithms:', algorithms);
  })
  .catch(error => console.error('Error:', error));
```

---

## CORS Support

All endpoints support Cross-Origin Resource Sharing (CORS). The following headers are set:

```
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, POST, OPTIONS
Access-Control-Allow-Headers: Content-Type
```

**OPTIONS Request**:

```bash
curl -X OPTIONS http://localhost:3000/api/predict \
  -H "Access-Control-Request-Method: POST" \
  -H "Access-Control-Request-Headers: Content-Type"
```

---

## Error Handling

All errors follow a consistent format:

```json
{
  "error": "Error message describing what went wrong"
}
```

### Common Error Scenarios

1. **Stock Not Found** (404):
```json
{
  "error": "Stock data not found for symbol: XYZ"
}
```

2. **Invalid Algorithm** (400):
```json
{
  "error": "Unknown algorithm: INVALID"
}
```

3. **Malformed JSON** (400):
```json
{
  "error": "Failed to parse JSON request body"
}
```

4. **Missing Required Fields** (400):
```json
{
  "error": "Missing required field: symbol"
}
```

---

## Rate Limiting

Currently, there is no rate limiting implemented. For production use, consider implementing rate limiting to prevent abuse.

---

## Authentication

Currently, the API does not require authentication. All endpoints are publicly accessible.

**⚠️ Security Note**: For production deployment, implement proper authentication and authorization mechanisms.

---

## Data Format

### Input CSV File Format

Stock data files should be named `{SYMBOL}.csv` and stored in the `data/` directory:

```csv
Date,Open,High,Low,Close,Volume
2025-11-01,150.0,155.0,149.0,153.0,1000000
2025-11-02,153.0,157.0,152.0,156.0,1200000
2025-11-03,156.0,159.0,155.0,158.0,1300000
```

**Required Columns**:
- Date: YYYY-MM-DD format
- Open: Opening price (numeric)
- High: Highest price (numeric)
- Low: Lowest price (numeric)
- Close: Closing price (numeric)
- Volume: Trading volume (numeric)

**Important Notes**:
- File naming: `AAPL.csv`, `MSFT.csv`, etc.
- Header row is required
- At least 5 data points needed for SMA algorithm
- No symbol column needed in the file

### Output Prediction CSV File Format

When predictions are generated, they are automatically saved to `{SYMBOL}_predictions.csv`:

```csv
Date,Predicted_Close
YYYY-MM-DD,153.82
YYYY-MM-DD,154.83
YYYY-MM-DD,155.75
YYYY-MM-DD,156.67
```

**Columns**:
- Date: Currently placeholder (YYYY-MM-DD)
- Predicted_Close: Predicted closing price

**Notes**:
- Files are overwritten on each prediction request
- Located in the `data/` directory
- SMA produces fewer predictions (data_points - window_size + 1)
- EMA produces predictions for all data points

---

## Complete Usage Example

Here's a complete workflow example:

```javascript
const API_BASE = 'http://localhost:3000';

// 1. Check server health
async function checkHealth() {
  const response = await fetch(`${API_BASE}/`);
  const data = await response.json();
  console.log('Server Status:', data.status);
  return data;
}

// 2. Get available algorithms
async function getAlgorithms() {
  const response = await fetch(`${API_BASE}/api/algorithms`);
  const algorithms = await response.json();
  console.log('Available Algorithms:', algorithms);
  return algorithms;
}

// 3. Get historical data
async function getHistoricalData(symbol) {
  const response = await fetch(`${API_BASE}/api/stocks/${symbol}`);
  if (!response.ok) {
    const error = await response.json();
    throw new Error(error.error);
  }
  const data = await response.json();
  console.log(`Historical data for ${symbol}:`, data);
  return data;
}

// 4. Get predictions
async function getPredictions(symbol, algorithm) {
  const response = await fetch(`${API_BASE}/api/predict`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ symbol, algorithm })
  });
  if (!response.ok) {
    const error = await response.json();
    throw new Error(error.error);
  }
  const predictions = await response.json();
  console.log(`Predictions for ${symbol} using ${algorithm}:`, predictions);
  return predictions;
}

// 5. Upload CSV and analyze
async function analyzeCustomCsv(file) {
  const formData = new FormData();
  formData.append('csv_file', file);
  
  const params = {
    algorithms: [
      { name: 'SMA', parameters: { window_size: 10 } },
      { name: 'EMA', parameters: { alpha: 0.25 } }
    ]
  };
  formData.append('params', JSON.stringify(params));
  
  const response = await fetch(`${API_BASE}/api/analyze`, {
    method: 'POST',
    body: formData
  });
  if (!response.ok) {
    const error = await response.json();
    throw new Error(error.error);
  }
  const result = await response.json();
  console.log('Analysis results:', result);
  return result;
}

// Run complete workflow
async function runWorkflow() {
  try {
    await checkHealth();
    const algorithms = await getAlgorithms();
    const historicalData = await getHistoricalData('AAPL');
    const predictions = await getPredictions('AAPL', algorithms[0]);
    
    // Example: analyze custom file (if you have a file input)
    // const fileInput = document.getElementById('csvFile');
    // if (fileInput.files[0]) {
    //   const analysisResult = await analyzeCustomCsv(fileInput.files[0]);
    //   console.log('Custom CSV analysis:', analysisResult);
    // }
  } catch (error) {
    console.error('Workflow error:', error.message);
  }
}

runWorkflow();
```

---

## Testing with cURL

Complete test suite using cURL:

```bash
# 1. Health check
curl http://localhost:3000/

# 2. Get algorithms
curl http://localhost:3000/api/algorithms

# 3. Get historical data
curl http://localhost:3000/api/stocks/AAPL

# 4. Get predictions with SMA
curl -X POST http://localhost:3000/api/predict \
  -H "Content-Type: application/json" \
  -d '{"symbol": "AAPL", "algorithm": "SMA"}'

# 5. Get predictions with EMA
curl -X POST http://localhost:3000/api/predict \
  -H "Content-Type: application/json" \
  -d '{"symbol": "MSFT", "algorithm": "EMA"}'

# 6. Upload CSV and analyze (all algorithms, default limit)
curl -X POST http://localhost:3000/api/analyze \
  -F "csv_file=@sample_stock.csv"

# Upload CSV with specific algorithm
curl -X POST http://localhost:3000/api/analyze \
  -F "csv_file=@sample_stock.csv" \
  -F "algorithm=SMA"

# Upload CSV with algorithm and limit
curl -X POST http://localhost:3000/api/analyze \
  -F "csv_file=@sample_stock.csv" \
  -F "algorithm=EMA" \
  -F "limit=5"

# 7. Test error handling (invalid symbol)
curl http://localhost:3000/api/stocks/INVALID

# 8. Test error handling (invalid algorithm)
curl -X POST http://localhost:3000/api/predict \
  -H "Content-Type: application/json" \
  -d '{"symbol": "AAPL", "algorithm": "INVALID"}'
```

---

## Support

For issues or questions about the API:
1. Check this documentation
2. Review the [README.md](README.md) for setup instructions
3. Open an issue in the repository

---

## Algorithm Configuration

### Simple Moving Average (SMA)

**Default Configuration**:
```json
{
  "window_size": 5,
  "min_window": 2,
  "max_window": 200
}
```

**Configuration in /api/analyze**:
```json
{
  "name": "SMA",
  "parameters": {
    "window_size": 10
  }
}
```

**Configurable Parameters**:
- `window_size` (integer): Number of days to include in the moving average
  - Minimum: 2
  - Maximum: 200
  - Default: 5
  - Recommendation: 5-20 for short-term, 50-200 for long-term trends

**How it works**:
1. Takes the last `window_size` closing prices
2. Calculates their arithmetic mean
3. Moves the window forward one day and repeats
4. Returns array of calculated averages

**Example with 5-day window**:
```
Input prices: [150, 151, 152, 153, 154, 155, 156, 157, 158, 159]
Window 1: (150+151+152+153+154)/5 = 152.0
Window 2: (151+152+153+154+155)/5 = 153.0
Window 3: (152+153+154+155+156)/5 = 154.0
...
Output: [152.0, 153.0, 154.0, 155.0, 156.0, 157.0]
```

### Exponential Moving Average (EMA)

**Default Configuration**:
```json
{
  "alpha": 0.2,
  "min_alpha": 0.0001,
  "max_alpha": 1.0
}
```

**Configuration in /api/analyze**:
```json
{
  "name": "EMA",
  "parameters": {
    "alpha": 0.3
  }
}
```

**Configurable Parameters**:
- `alpha` (float): Smoothing factor determining weight of recent vs. historical prices
  - Minimum: 0.0001
  - Maximum: 1.0
  - Default: 0.2
  - Higher values (0.5-0.9): More responsive to recent changes
  - Lower values (0.1-0.3): Smoother, less reactive
  - Very low (0.01-0.1): Similar to SMA with large window

**How it works**:
1. Starts with first closing price as initial EMA
2. For each subsequent price: `EMA[i] = α × price[i] + (1-α) × EMA[i-1]`
3. Higher α (e.g., 0.8) = more weight to recent prices (more responsive)
4. Lower α (e.g., 0.1) = more weight to historical average (smoother)

**Example with α=0.2**:
```
Input prices: [150, 151, 152, 153, 154]
EMA[0] = 150
EMA[1] = 0.2 × 151 + 0.8 × 150 = 150.2
EMA[2] = 0.2 × 152 + 0.8 × 150.2 = 150.56
EMA[3] = 0.2 × 153 + 0.8 × 150.56 = 150.848
EMA[4] = 0.2 × 154 + 0.8 × 150.848 = 151.478
Output: [150, 150.2, 150.56, 150.848, 151.478]
```

### Parameter Validation

The server automatically validates all parameters:

**Validation Response Example**:
```json
{
  "validations": [
    {
      "algorithm": "SMA",
      "parameters": {
        "window_size": 10,
        "min_window": 2,
        "max_window": 200
      }
    }
  ]
}
```

**Validation Errors**:
```json
{
  "errors": [
    {
      "algorithm": "SMA",
      "error": "Window size must be between 2 and 200"
    }
  ]
}
```

### Choosing Algorithm Parameters

**For Short-term Trading (Days to Weeks)**:
- SMA: `window_size: 5-20`
- EMA: `alpha: 0.3-0.5`

**For Medium-term Trends (Weeks to Months)**:
- SMA: `window_size: 20-50`
- EMA: `alpha: 0.15-0.3`

**For Long-term Analysis (Months to Years)**:
- SMA: `window_size: 50-200`
- EMA: `alpha: 0.05-0.15`

**Volatility Considerations**:
- High volatility stocks: Use larger windows (SMA) or lower alpha (EMA) for stability
- Low volatility stocks: Use smaller windows (SMA) or higher alpha (EMA) for responsiveness

---

## Testing Endpoints with PowerShell

```powershell
# 1. Health Check
Invoke-RestMethod -Uri http://localhost:3000/

# 2. Get Algorithms
Invoke-RestMethod -Uri http://localhost:3000/api/algorithms

# 3. Get AAPL Stock Data
Invoke-RestMethod -Uri http://localhost:3000/api/stocks/AAPL

# 4. Get MSFT Stock Data
Invoke-RestMethod -Uri http://localhost:3000/api/stocks/MSFT

# 5. Predict AAPL with SMA
Invoke-RestMethod -Uri http://localhost:3000/api/predict `
  -Method POST -ContentType "application/json" `
  -Body '{"symbol":"AAPL","algorithm":"SMA"}'

# 6. Predict MSFT with EMA
Invoke-RestMethod -Uri http://localhost:3000/api/predict `
  -Method POST -ContentType "application/json" `
  -Body '{"symbol":"MSFT","algorithm":"EMA"}'

# 7. Upload and analyze CSV file (all algorithms)
Invoke-RestMethod -Uri http://localhost:3000/api/analyze `
  -Method POST `
  -Form @{
      csv_file = Get-Item -Path "sample_stock.csv"
  }

# Upload with specific algorithm and limit
Invoke-RestMethod -Uri http://localhost:3000/api/analyze `
  -Method POST `
  -Form @{
      csv_file = Get-Item -Path "sample_stock.csv"
      algorithm = "SMA"
      limit = "15"
  }

# 8. Test Error Handling
try {
  Invoke-RestMethod -Uri http://localhost:3000/api/stocks/INVALID
} catch {
  Write-Host "Expected 404 Error: $_"
}
```

---

## Performance Considerations

- **Response Time**: 
  - Health check: ~5-10ms
  - Historical data: ~20-50ms (depends on file size)
  - Predictions: ~30-80ms (depends on algorithm and data size)

- **Data Size Limits**:
  - Recommended: Up to 1000 data points per symbol
  - Maximum tested: 10,000 data points

- **Concurrent Requests**: 
  - Server supports multiple concurrent connections
  - No request queuing or rate limiting currently implemented

---

## Common Use Cases

### 1. Build a Stock Dashboard
```javascript
// Fetch data for multiple stocks
const symbols = ['AAPL', 'MSFT', 'GOOGL'];
const promises = symbols.map(symbol => 
  fetch(`http://localhost:3000/api/stocks/${symbol}`)
    .then(r => r.json())
);
const allData = await Promise.all(promises);
```

### 2. Compare Algorithm Performance
```javascript
// Get predictions from both algorithms
const smaResult = await fetch('http://localhost:3000/api/predict', {
  method: 'POST',
  headers: {'Content-Type': 'application/json'},
  body: JSON.stringify({symbol: 'AAPL', algorithm: 'SMA'})
}).then(r => r.json());

const emaResult = await fetch('http://localhost:3000/api/predict', {
  method: 'POST',
  headers: {'Content-Type': 'application/json'},
  body: JSON.stringify({symbol: 'AAPL', algorithm: 'EMA'})
}).then(r => r.json());

console.log('SMA:', smaResult.predictions);
console.log('EMA:', emaResult.predictions);
```

### 3. Upload and Analyze Custom Data
```javascript
// Upload CSV file and run all algorithms
const formData = new FormData();
formData.append('csv_file', fileInput.files[0]);
formData.append('limit', '20');  // Get 20 predictions from each algorithm

const result = await fetch('http://localhost:3000/api/analyze', {
  method: 'POST',
  body: formData
}).then(r => r.json());

// Compare results from all algorithms
console.log('All predictions:', result.predictions);
console.log('SMA predictions:', result.predictions.SMA);
console.log('EMA predictions:', result.predictions.EMA);

// Or run specific algorithm
const formData2 = new FormData();
formData2.append('csv_file', fileInput.files[0]);
formData2.append('algorithm', 'SMA');
formData2.append('limit', '10');

const result2 = await fetch('http://localhost:3000/api/analyze', {
  method: 'POST',
  body: formData2
}).then(r => r.json());

console.log('SMA only:', result2.predictions);
```

### 4. Automated Trading Bot Integration
```python
import requests
import time

API_BASE = "http://localhost:3000"

def get_prediction(symbol, algorithm):
    response = requests.post(
        f"{API_BASE}/api/predict",
        json={"symbol": symbol, "algorithm": algorithm}
    )
    return response.json()

# Monitor and predict every hour
while True:
    prediction = get_prediction("AAPL", "EMA")
    print(f"Latest prediction: {prediction['predictions'][-1]}")
    time.sleep(3600)  # Wait 1 hour
```

---

**Last Updated**: November 6, 2025
**Version**: 1.0.0
**Docker Support**: ✅ Fully Containerized
