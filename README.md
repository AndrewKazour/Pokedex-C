# 🔴 Pokedex-C

A Pokedex web application written in C, featuring all 151 original Pokemon with a built-in HTTP server.

![C](https://img.shields.io/badge/Language-C-blue)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey)

---

## ✨ Features

- 📖 Browse all 151 Generation 1 Pokemon
- 🔍 Search by name or ID
- ✅ Track Pokemon as "Seen" or "Caught"
- 💾 Progress saved automatically
- 🌐 Web-based UI served by C backend

---

## 📁 Project Structure

```
Pokedex-C/
├── include/
│   └── pokemon.h          # Data structures & function declarations
├── src/
│   ├── main.c             # Entry point - starts the server
│   ├── file_io.c          # CSV parsing & progress file I/O
│   ├── search.c           # Binary Search Tree for name lookup
│   ├── progress.c         # Seen/caught tracking
│   ├── json.c             # JSON generation for API
│   └── http_server.c      # HTTP request handling
├── pokemon_data.csv       # Pokemon database
├── pokedex.html           # Web frontend
├── Getpokemondata.py      # Script to fetch Pokemon data
└── Makefile               # Build automation
```

---

## 🛠️ Prerequisites

- **GCC** (MinGW on Windows)
- **Make** (`mingw32-make` on Windows)
- **Python 3** (only for regenerating Pokemon data)

---

## 🚀 Quick Start

### 1. Build the project
```bash
mingw32-make
```

### 2. Run the server
```bash
.\pokedex_server.exe
```

### 3. Open in browser
```
http://localhost:8080
```

---

## 📋 Commands

| Command | Description |
|---------|-------------|
| `mingw32-make` | Build the project |
| `mingw32-make run` | Build and run |
| `mingw32-make clean` | Remove executable |
| `mingw32-make rebuild` | Clean and rebuild |

---

## 🔄 Reset Progress

Reset **all** progress:
```bash
.\pokedex_server.exe --reset
```

Reset a **specific** Pokemon (e.g., #25 Pikachu):
```bash
.\pokedex_server.exe --reset-id 25
```

Show help:
```bash
.\pokedex_server.exe --help
```

---

## 🌐 API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Web interface |
| `/api/list` | GET | Get all Pokemon |
| `/api/search?id=25` | GET | Search by ID |
| `/api/search?q=pikachu` | GET | Search by name |
| `/api/progress` | GET | Get seen/caught totals |
| `/api/encounter?id=25` | GET | Mark as seen |
| `/api/catch?id=25` | GET | Mark as caught |
| `/api/reset?id=25` | GET | Reset one Pokemon |
| `/api/reset-all` | GET | Reset all progress |

---

## 🐍 Regenerating Pokemon Data

If you need to refresh the Pokemon data:

```bash
pip install requests
python Getpokemondata.py
```

This fetches data from PokeAPI and generates `pokemon_data.csv`.

---

## 📝 License

This project is for educational purposes.