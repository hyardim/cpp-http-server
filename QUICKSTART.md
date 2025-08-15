# 🚀 Quick Start Guide

Get your C++ HTTP server running in under 2 minutes!

## ⚡ Super Quick Start

1. **Build the server:**
   ```bash
   make
   ```

2. **Run the server:**
   ```bash
   ./bin/http_server
   ```

3. **Test it:**
   - Open your browser to: http://localhost:8080
   - Or use curl: `curl http://localhost:8080/health`

That's it! 🎉

## 🔧 Customization

### Change Port
```bash
./bin/http_server --port 3000
```

### Run in Background
```bash
./bin/http_server &
```

### View Help
```bash
./bin/http_server --help
```

## 🧪 Test Everything

Run the automated test script:
```bash
./test_server.sh
```

## 📁 Project Structure

```
HTTP Server/
├── include/           # Header files
├── src/              # Source files
├── build/            # Build artifacts (auto-created)
├── bin/              # Executable (auto-created)
├── CMakeLists.txt    # CMake configuration
├── Makefile          # Make build system
├── README.md         # Full documentation
├── QUICKSTART.md     # This file
└── test_server.sh    # Test script
```

## 🐛 Common Issues

**"Permission denied" on port 8080:**
- Use a higher port: `./bin/http_server --port 8081`

**"Address already in use":**
- Kill existing process: `lsof -ti:8080 | xargs kill`
- Or use different port

**Build errors:**
- Ensure you have g++ installed
- Check C++17 support: `g++ --version`

## 🌟 Next Steps

- Read the full [README.md](README.md) for detailed documentation
- Explore the code in `include/` and `src/` directories
- Add your own routes in `route_handler.cpp`
- Customize the server behavior

## 💡 Pro Tips

- Use `make debug` for development builds
- Check `server.log` for debugging information
- The server gracefully handles Ctrl+C shutdown
- All endpoints support CORS for web development

---

**Need help?** Check the full [README.md](README.md) or run `./bin/http_server --help` 