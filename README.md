# ScribeHub 🚀

[![C/C++ CI](https://github.com/yourusername/scribehub/actions/workflows/ci.yml/badge.svg)](https://github.com/yourusername/scribehub/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Docker Image](https://img.shields.io/badge/Docker-ready-blue.svg)](Dockerfile)

A production-quality, fast, and feature-rich terminal text editor written in C using **ncurses**.

ScribeHub has been modernized from a single-file college project into a robust, modular, and fully unit-tested terminal application ready for developer contribution.

---

## 📖 Table of Contents
- [Motivation](#-motivation)
- [Features](#-features)
- [Architecture & Design](#-architecture--design)
- [Tech Stack](#-tech-stack)
- [Installation & Getting Started](#-installation--getting-started)
- [Running Locally](#-running-locally)
- [Containerization with Docker](#-containerization-with-docker)
- [Project Structure](#-project-structure)
- [Testing](#-testing)
- [Key Bindings](#-key-bindings)
- [Future Roadmap](#-future-roadmap)

---

## 🎯 Motivation

The main goal of ScribeHub's modernization is to demonstrate how standard production engineering practices can turn an unstructured academic project into a clean, maintainable, and reliable open-source codebase.

By separating **Core Business Logic** (text storage/manipulation) from **User Interface Presentation** (ncurses backend), ScribeHub achieves decoupled design, eliminates buffer overflow vulnerabilities, enables comprehensive automated test suites, and implements modern CI/CD practices.

---

## ✨ Features

- 📁 **File Operations:** Seamlessly open, edit, save, and rename files.
- 🎨 **Multiple Themes:** Effortlessly switch between Default, Green, Yellow, and Notepad themes.
- 📝 **Bold Formatting:** Highlight specific rows with bold text state toggle.
- 🔍 **Search & Replace:** Safe multi-word find-and-replace with buffer overflow protection.
- 🕒 **Autosave Function:** Prevent data loss with optional automatic background saves.
- 🧪 **Unit Test Suite:** Fully automated tests validating buffer and state modification.
- 🐳 **Docker Support:** Runs cleanly inside containerized virtual terminal environments.

---

## 🏗️ Architecture & Design

ScribeHub follows a strict modular structure loosely inspired by the **Model-View-Controller (MVC)** pattern:

```
┌────────────────────────────────────────────────────────┐
│                        MAIN.C                          │
│          App Entry, Arg Parsing, Allocations           │
└───────────┬────────────────────────────────┬───────────┘
            │                                │
            ▼                                ▼
┌──────────────────────┐          ┌──────────────────────┐
│       UI.C/.H        │          │     BUFFER.C/.H      │
│  (View/Presentation) │          │    (Model/Logic)     │
│   ncurses rendering  │◄─────────┤  Line allocations,   │
│   Keyboard inputs    │          │  Insert, Delete, F&R │
└──────────────────────┘          └──────────────────────┘
```

- **`src/buffer.c`**: Implements the memory allocation, line structures, character manipulation, text insertion/deletion, search & replace, and save/load utilities. It is **100% free of UI dependencies**, which allows it to be compiled and fully unit-tested in isolation!
- **`src/ui.c`**: Handles screen layout, rendering logic, key input translation, and multi-theme ncurses color pairing.
- **`src/main.c`**: Manages the application runtime, safe argv copy buffer instantiation, and exit sequence cleanup.

---

## 💻 Tech Stack

- **Language:** ISO C99
- **Libraries:** standard `ncurses` (for robust window creation and color profiles)
- **Containerization:** Docker (multi-stage minimal build)
- **Automated Testing:** Native custom C testing harness
- **CI/CD:** GitHub Actions

---

## 📥 Installation & Getting Started

### Prerequisites

ScribeHub requires standard development tools and `ncurses`.

#### On Ubuntu / Debian:
```bash
sudo apt update
sudo apt install -y build-essential libncurses5-dev libncursesw5-dev
```

#### On macOS (using Homebrew):
```bash
brew install ncurses
```

---

## 🚀 Running Locally

Clone the repository and compile using the provided Makefile:

```bash
# Compile ScribeHub
make

# Run ScribeHub with a target text file
./scribehub notes.txt
```

---

## 🐳 Containerization with Docker

ScribeHub supports multi-stage lightweight builds. You can compile and run the application completely within a Docker container:

```bash
# Build the Docker image
docker build -t scribehub .

# Run inside the container (attaching interactive terminal input & output)
docker run -it --name scribehub-app scribehub my_docker_notes.txt
```

---

## 📂 Project Structure

```
.
├── .github/
│   └── workflows/
│       └── ci.yml             # Automated build and test pipeline
├── src/
│   ├── buffer.c               # Text buffer management & manipulation
│   ├── buffer.h               # Core logic definitions
│   ├── config.h               # Limits, constraints, key binding definitions
│   ├── ui.c                   # Terminal styling and input loops
│   ├── ui.h                   # Terminal display abstraction interface
│   └── main.c                 # Application entry point
├── tests/
│   └── test_buffer.c          # Unit test assertions (no UI dependencies)
├── Dockerfile                 # Multi-stage lightweight docker image
├── Makefile                   # Clean, test, compile, and format build directives
├── .editorconfig              # Editor consistency rules
├── .clang-format              # Formatting standard definitions
└── README.md                  # Comprehensive documentation
```

---

## 🧪 Testing

We believe that reliable software requires comprehensive tests. ScribeHub's core logic contains a full suite of unit tests validating boundary conditions, search and replace correctness, line split constraints, and memory operations.

To run the test suite:
```bash
make test
```

---

## 🎹 Key Bindings

| Key Combination | Action |
|---|---|
| `Ctrl + A` | Save and exit |
| `Ctrl + P` | Save without exiting |
| `Ctrl + O` | Exit without saving |
| `Ctrl + T` | Toggle Autosave on/off |
| `Ctrl + R` | Rename the current file safely |
| `Ctrl + Y` | Safe Find & Replace |
| `Ctrl + [` | Toggle bold weight on active line |
| `Ctrl + N` | Toggle Green Theme |
| `Ctrl + E` | Toggle Yellow Theme |
| `Ctrl + G` | Toggle Notepad Theme |
| `Ctrl + W` | Display interactive keybindings guide |

---

## 🔮 Future Roadmap

- [ ] **Dynamic Buffer Scaling:** Auto-grow `MAX_LINES` and `MAX_LINE_LENGTH` to handle infinitely large text files.
- [ ] **Multi-file Buffers:** Allow switching between multiple open documents concurrently using tabs.
- [ ] **Undo / Redo History:** Implement a robust command pattern stack to support infinite undo and redo.
- [ ] **Syntax Highlighting:** Add basic regex-based syntax coloring for popular markup and programming languages.
