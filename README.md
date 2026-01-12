# AI Code Editor

An AI-powered C/C++ code editor for Windows 10/11 with local AI integration.

## Features

- **Modern Dark Theme UI** - Windows 10+ styled interface with dark mode
- **C/C++ Code Editor** - Syntax highlighting, line numbers, auto-indentation
- **AI Chat Assistant** - Get help with your code from a local AI
- **Follow-up Questions** - Continue conversations with the AI
- **Ideas & Suggestions** - Get AI-powered code improvement suggestions
- **Compiler Integration** - Compile and run your code directly
- **Multiple Compiler Support** - GCC, Clang, MSVC, MinGW

## Requirements

### Build Requirements
- CMake 3.16 or higher
- Qt 6.2 or higher
- C++17 compatible compiler (MSVC 2019+, GCC 9+, Clang 10+)

### Runtime Requirements
- Windows 10 or higher
- A C/C++ compiler (GCC/MinGW, Clang, or MSVC)
- Ollama (for AI features) - https://ollama.ai

## Building

### Windows (with MSVC)

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:/Qt/6.6.0/msvc2019_64"
cmake --build . --config Release
```

### Windows (with MinGW)

```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.6.0/mingw_64"
cmake --build .
```

## Setting Up Local AI

1. Install Ollama from https://ollama.ai
2. Pull a code-focused model:
   ```bash
   ollama pull codellama
   ```
3. Ollama runs automatically in the background on port 11434

### Recommended Models
- `codellama` - Best for code assistance (default)
- `llama2` - General purpose
- `mistral` - Fast and capable
- `deepseek-coder` - Specialized for coding

## Usage

1. **Write Code** - Use the left panel to write your C/C++ code
2. **Ask AI** - Use the Chat tab to ask questions about your code
3. **Get Suggestions** - Click "Get Suggestions" in the Ideas tab
4. **Compile** - Press F5 or click "Build & Run" to compile and execute

### Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| Ctrl+N | New file |
| Ctrl+O | Open file |
| Ctrl+S | Save file |
| Ctrl+B | Compile |
| Ctrl+R | Run |
| F5 | Compile & Run |
| Ctrl+I | Toggle AI Panel |

## Architecture

```
src/
├── main.cpp              # Application entry point
├── mainwindow.h/cpp      # Main application window
├── codeeditor.h/cpp      # Code editor with line numbers
├── syntaxhighlighter.h/cpp # C/C++ syntax highlighting
├── aichatpanel.h/cpp     # AI chat interface with tabs
├── aiservice.h/cpp       # Ollama API integration
└── compilerservice.h/cpp # Compiler integration
```

## License

MIT License
