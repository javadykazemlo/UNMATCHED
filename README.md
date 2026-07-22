# ⚔️ UNMATCHED

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.10+-green.svg)](https://cmake.org/)
[![FTXUI](https://img.shields.io/badge/UI-FTXUI-orange.svg)](https://github.com/ArthurSonzogni/FTXUI)
[![Platform](https://img.shields.io/badge/Platform-Terminal-lightgrey.svg)]()
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

> A C++ implementation of the popular board game **Unmatched**, developed as the final project for the **Advanced Programming** course at **Bu-Ali Sina University**.

---

# 🗂 Table of Contents

- [⚔️ UNMATCHED](#️-unmatched)
  - [🗂 Table of Contents](#-table-of-contents)
  - [📖 About the Game](#-about-the-game)
  - [✨ Features](#-features)
  - [👥 Team Members](#-team-members)
  - [📂 Project Structure](#-project-structure)
  - [📥 Installation](#-installation)
    - [Prerequisites](#prerequisites)
  - [🎮 How to Play](#-how-to-play)
  - [🛠️ Technologies Used](#️-technologies-used)
  - [🎯 Programming Concepts](#-programming-concepts)
  - [🚀 Future Improvements](#-future-improvements)
  - [👭 Contributing](#-contributing)

---

# 📖 About the Game

**UNMATCHED** is a digital implementation of the famous strategy board game developed by **Restoration Games**.

The game brings legendary characters from literature and mythology into exciting one-on-one battles. Every hero owns a completely different deck of cards, unique abilities, and loyal sidekicks, making every match feel different and highly strategic.

This project was developed in **Modern C++** as the final project for the **Advanced Programming** course. It focuses on applying object-oriented programming principles, modular software design, and clean architecture while recreating the gameplay experience inside a terminal interface.

Unlike traditional action games, every decision matters. Players must carefully choose how to move, when to attack, and which cards to play in order to defeat their opponent.

The current version focuses on recreating the battle between **Sherlock Holmes** and **Dracula**, each with their own cards, abilities, and companions.

---

## 🎯 Game Concept

| Property | Description |
|----------|-------------|
| **Genre** | Strategy Board Game |
| **Mode** | Local Multiplayer |
| **Players** | 2 Players |
| **Interface** | Terminal (FTXUI) |
| **Language** | C++17 |
| **Build System** | CMake |
| **Library** | FTXUI |

---


# ✨ Features

The project recreates the core gameplay mechanics of **Unmatched** while emphasizing clean C++ design and modular development.

## 🎮 Gameplay Features

| Feature | Description |
|---------|-------------|
| ⚔️ Turn-Based Gameplay | Players alternate turns throughout the game |
| 🃏 Card-Based Combat | Every action is driven by hero cards |
| 👥 Two Unique Heroes | Sherlock Holmes and Dracula |
| 🤝 Sidekick Support | Heroes fight alongside their companions |
| 🗺️ Interactive Board | Characters move across the battlefield |
| ❤️ Health System | Heroes and sidekicks have independent health |
| 🎯 Tactical Decisions | Movement, attacks and card management determine victory |

---

## 💻 Technical Features

- Modern C++17
- Object-Oriented Programming
- Modular Project Structure
- Separation of Headers and Source Files
- CMake Build System
- Terminal User Interface using FTXUI
- Cross-platform support
- Easy-to-extend architecture
- Clean class hierarchy
- Reusable game components

---

## 🎨 User Interface

The project uses **FTXUI** to provide a modern terminal interface.

Features include:

- Interactive menus
- Colored interface
- Game dashboard
- Board visualization
- Player information
- Hero statistics
- Card visualization
- Turn information

The interface is completely terminal-based while remaining clean and user-friendly.

---

# 👥 Team Members

| Name | Student Number | GitHub |
|------|---------------|--------|
| **Mahdi Dehnavi** | 40412358018 | https://github.com/mahdidehnavi |
| **Mohammad Javad Kazemlo** | 40412358038 | https://github.com/javadykazemlo |



---

# 📂 Project Structure

The project follows a clean modular architecture that separates the game into different logical components.

```text
.
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include
│   ├── GameTUI.hpp
│   │
│   ├── cards
│   │   ├── Card.hpp
│   │   └── Deck.hpp
│   │
│   ├── core
│   │   ├── Bord.hpp
│   │   ├── Controller.hpp
│   │   └── Player.hpp
│   │
│   └── entities
│       ├── Character.hpp
│       ├── Sherlock.hpp
│       ├── Dr_watson.hpp
│       ├── Dracula.hpp
│       └── Sister.hpp
│
├── src
│   ├── GameTUI.cpp
│   ├── main.cpp
│   │
│   ├── cards
│   │   ├── Card.cpp
│   │   └── Deck.cpp
│   │
│   ├── core
│   │   ├── Bord.cpp
│   │   ├── Controller.cpp
│   │   ├── Effects.cpp
│   │   └── Player.cpp
│   │
│   └── entities
│       ├── Character.cpp
│       ├── Sherlock.cpp
│       ├── Dr_watson.cpp
│       ├── Dracula.cpp
│       └── Sister.cpp
│
└── external
    └── ftxui
```

# 📥 Installation

## Prerequisites

Before building the project, make sure the following software is installed.

- C++17 Compiler
- CMake 3.10+
- Git

The project automatically builds with **FTXUI** through CMake.

---

## Clone Repository

```bash
git clone https://github.com/javadykazemlo/UNMATCHED.git
```
# 🎮 How to Play

UNMATCHED is a **two-player strategy game** where each player controls a legendary hero with a unique deck of cards and loyal sidekicks.

The objective is simple:

> Defeat the opposing hero before your own hero is eliminated.

Every decision during the game matters. Players must carefully manage movement, attacks, defense, and card usage to gain the upper hand.

---

---

# 🛠️ Technologies Used

| Technology | Purpose |
|------------|---------|
| C++17 | Programming Language |
| CMake | Build System |
| FTXUI | Terminal User Interface |
| Git | Version Control |
| GitHub | Source Code Hosting |

---

# 🎯 Programming Concepts

This project applies many important software engineering concepts, including:

- Object-Oriented Programming (OOP)
- Inheritance
- Polymorphism
- Encapsulation
- Modular Design
- Separation of Concerns
- Class Hierarchy
- STL Containers
- CMake Project Structure

These concepts were used to create a maintainable and extensible implementation of the game.

---

# 🚀 Future Improvements

Possible future extensions include:

- Additional Heroes
- Additional Maps
- Artificial Intelligence (AI)
- Save & Load Game
- Sound Effects
- Animations
- Online Multiplayer
- Improved Card Effects
- Expanded Hero Decks
- Better Terminal Graphics

The modular architecture makes adding new content significantly easier.

---

# 👭 Contributing

Contributions are welcome!

If you would like to improve the project:

1. Fork the repository.
2. Create a new branch.

```bash
git checkout -b feature/YourFeature
```

3. Commit your changes.
4. 
## ⭐ If you like this project...

Give it a ⭐ on GitHub and help others discover it.

Happy Coding! 🚀
