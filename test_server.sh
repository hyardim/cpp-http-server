#!/bin/bash

# Test script for C++ HTTP Server
# This script demonstrates the server's capabilities

echo "C++ HTTP Server Test Script"
echo "================================"
echo

# Check if server binary exists
if [ ! -f "./bin/http_server" ]; then
    echo "Server binary not found. Please build the project first:"
    echo "   make"
    exit 1
fi

# Function to test an endpoint
test_endpoint() {
    local method=$1
    local endpoint=$2
    local data=$3
    local description=$4
    
    echo "🔍 Testing: $description"
    echo "   $method $endpoint"
    
    if [ "$method" = "POST" ] && [ -n "$data" ]; then
        response=$(curl -s -X POST -d "$data" "http://localhost:8080$endpoint")
    else
        response=$(curl -s "http://localhost:8080$endpoint")
    fi
    
    if [ $? -eq 0 ]; then
        echo "   Success"
        echo "   Response preview:"
        echo "$response" | head -3 | sed 's/^/      /'
        if [ $(echo "$response" | wc -l) -gt 3 ]; then
            echo "      ... (truncated)"
        fi
    else
        echo "   Failed"
    fi
    echo
}

# Start server in background
echo "Starting HTTP server on port 8080..."
./bin/http_server --port 8080 > server.log 2>&1 &
SERVER_PID=$!

# Wait for server to start
echo " Waiting for server to start..."
sleep 3

# Check if server is running
if ! curl -s http://localhost:8080/health > /dev/null 2>&1; then
    echo " Server failed to start. Check server.log for details."
    kill $SERVER_PID 2>/dev/null
    exit 1
fi

echo "✅ Server is running (PID: $SERVER_PID)"
echo

# Test endpoints
test_endpoint "GET" "/" "" "Root endpoint - Server information"
test_endpoint "GET" "/health" "" "Health check endpoint"
test_endpoint "GET" "/echo?name=test&value=123" "" "Echo endpoint with query parameters"
test_endpoint "POST" "/echo" "Hello from test script!" "Echo endpoint with POST data"
test_endpoint "GET" "/nonexistent" "" "Non-existent endpoint (should return 404)"

echo " Testing complete!"
echo

# Stop server
echo " Stopping server..."
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo " Server stopped"
echo
echo "📋 Server log (last 10 lines):"
echo "--------------------------------"
tail -10 server.log 2>/dev/null || echo "No log file found"

echo
echo "Test completed successfully!"
echo "You can also test manually by running: ./bin/http_server"
echo "Then visit http://localhost:8080 in your browser" 