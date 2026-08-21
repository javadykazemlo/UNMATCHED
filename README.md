# ⚔️ UNMATCHED

[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.10%2B-green.svg)](https://cmake.org/)
[![SFML](https://img.shields.io/badge/SFML-3-orange.svg)](https://www.sfml-dev.org/)
[![Platform](https://img.shields.io/badge/Platform-Desktop-lightgrey.svg)]()

> A C++ implementation of the board game **Unmatched**, developed as a final project for the Advanced Programming course at Bu-Ali Sina University.

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
    - [Build](#build)
    - [Run](#run)
  - [🎮 How to Play](#-how-to-play)
  - [🛠️ Technologies Used](#️-technologies-used)
  - [🎯 Programming Concepts](#-programming-concepts)
  - [🚀 Future Improvements](#-future-improvements)
  - [👭 Contributing](#-contributing)

---

# 📖 About the Game

**UNMATCHED** is a digital implementation of the strategy board game developed by **Restoration Games**.

The project recreates a two-player Unmatched-style match with heroes, sidekicks, cards, movement, combat, schemes, hero abilities, and a graphical game interface. The current implementation includes **Dracula**, **Sherlock Holmes**, and **Invisible Man**, together with their supported sidekicks.

The project is implemented in **C++17** with a modular object-oriented architecture. Game rules and state are handled by the core classes, while the graphical layer is responsible for presenting the game and collecting GUI input.

---

## 🎯 Game Concept

| Property | Description |
|----------|-------------|
| **Genre** | Strategy Board Game |
| **Mode** | Local Multiplayer / Player vs AI |
| **Players** | 2 Players |
| **Interface** | SFML graphical interface |
| **Language** | C++17 |
| **Build System** | CMake |
| **Graphics / Audio** | SFML 3 |

---

# ✨ Features

## 🎮 Gameplay Features

| Feature | Description |
|---------|-------------|
| ⚔️ Turn-Based Gameplay | Players alternate turns and take actions during their turn |
| 🃏 Card-Based Combat | Attack, defense, boost, and scheme cards drive the main gameplay |
| 🧛 Multiple Heroes | Dracula, Sherlock Holmes, and Invisible Man are supported |
| 🤝 Sidekicks | Heroes can fight together with their associated sidekicks |
| 🗺️ Interactive Board | Fighters move through the board's connected spaces and zones |
| ❤️ Health System | Heroes and sidekicks have independent health values |
| 🎯 Combat Effects | Card effects and combat modifiers are resolved by the game logic |
| 🧩 Hero Abilities | Supported heroes can use their implemented special abilities |
| 🤖 AI Opponent | A built-in AI can make gameplay decisions for the second player |
| 💾 Save / Load | Games can be saved and loaded through JSON save files |

## 🖥️ Graphical Features

The game uses **SFML 3** for its graphical interface.

The current GUI includes:

- Main menu and new-game flow
- Player information setup
- Hero selection
- Hero starting-position selection
- Sidekick placement
- Interactive game board
- Character and sidekick tokens
- Character and card information panels
- Hand/deck/discard visualization
- Move, attack, scheme, save, and turn controls
- Attack and scheme interaction panels
- Dracula ability interaction panel
- AI turn panel
- Game-over screen
- Save/load screen
- Rules screen
- Hover and click feedback
- Background music and sound effects
- Music mute control

The graphical layer communicates with the controller through dedicated GUI methods instead of replacing the underlying game rules.

---

# 👥 Team Members

| Name | Student Number | GitHub |
|------|---------------|--------|
| **Mahdi Dehnavi** | 40412358018 | https://github.com/mahdidehnavi |
| **Mohammad Javad Kazemlo** | 40412358038 | https://github.com/javadykazemlo |

---

# 📂 Project Structure

The project is organized into separate modules for game rules, entities, cards, graphics, AI, and save/load functionality.

```text
.
├── CMakeLists.txt
├── README.md
├── include
│   ├── cards
│   │   ├── Card.hpp
│   │   └── Deck.hpp
│   │
│   ├── core
│   │   ├── Bord.hpp
│   │   ├── Controller.hpp
│   │   ├── GameAI.hpp
│   │   ├── GuiEffectLogger.hpp
│   │   └── Player.hpp
│   │
│   ├── entities
│   │   ├── Character.hpp
│   │   ├── Dracula.hpp
│   │   ├── Dr_watson.hpp
│   │   ├── invisible_man.hpp
│   │   ├── Sherlock.hpp
│   │   └── Sister.hpp
│   │
│   ├── graphics
│   │   ├── AudioManager.hpp
│   │   ├── BoardView.hpp
│   │   ├── CardView.hpp
│   │   ├── CharacterView.hpp
│   │   ├── DeckView.hpp
│   │   ├── GameWindow.hpp
│   │   ├── RulesView.hpp
│   │   ├── TextureManager.hpp
│   │   └── UI.hpp
│   │
│   ├── Save
│   │   └── SaveManager.hpp
│   │
│   └── nlohmann
│       └── json.hpp
│
├── src
│   ├── main.cpp
│   ├── cards
│   │   ├── Card.cpp
│   │   └── Deck.cpp
│   │
│   ├── core
│   │   ├── Bord.cpp
│   │   ├── Controller.cpp
│   │   ├── Effects.cpp
│   │   ├── GameAI.cpp
│   │   └── Player.cpp
│   │
│   ├── entities
│   │   ├── Character.cpp
│   │   ├── Dracula.cpp
│   │   ├── Dr_watson.cpp
│   │   ├── invisible_man.cpp
│   │   ├── Sherlock.cpp
│   │   └── Sister.cpp
│   │
│   ├── graphics
│   │   ├── AudioManager.cpp
│   │   ├── BoardView.cpp
│   │   ├── CardView.cpp
│   │   ├── CharacterView.cpp
│   │   ├── DeckView.cpp
│   │   ├── GameWindow.cpp
│   │   ├── RulesView.cpp
│   │   ├── TextureManager.cpp
│   │   └── UI.cpp
│   │
│   └── Save
│       └── SaveManager.cpp
│
└── assets
    ├── images
    ├── audio
    └── fonts
```

---

# 📥 Installation

## Prerequisites

Install the following:

- C++17-compatible compiler
- CMake 3.10 or newer
- SFML 3 with Graphics, Window, System, and Audio components
- Git

The project expects the `assets` directory to be present beside the source tree when the executable is run. The repository's CMake configuration also copies `assets` next to the built executable after a successful build.

---

## Clone Repository

```bash
git clone https://github.com/javadykazemlo/UNMATCHED.git
cd UNMATCHED
```

## Build

Create a separate build directory:

```bash
cmake -S . -B build
cmake --build build
```

On systems where SFML is installed in a custom location, provide its CMake prefix:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/SFML
cmake --build build
```

On Windows, the current CMake configuration also supports the SFML installation path used by the project and copies required SFML DLLs beside the executable when that installation exists.

## Run

After a successful build, run the generated executable from the build directory:

```bash
./build/a
```

The exact executable location can differ depending on the generator and platform.

---

# 🎮 How to Play

UNMATCHED is a two-player strategy game. Each player controls a Hero and their associated Sidekick(s).

The general objective is:

> Defeat the opposing Hero before your own Hero is defeated.

### Game Setup

1. Enter the player information.
2. Select the Heroes.
3. Select each Hero's starting position.
4. Place the Sidekicks in their available starting spaces.
5. Start the match.

### During a Turn

A player has two actions. The available GUI actions are:

- **MOVE** — perform a maneuver and handle the movement flow.
- **ATTACK** — select an attacker, target, and combat cards, then resolve combat.
- **SCHEME** — play an eligible Scheme card and resolve its effect.
- **SAVE GAME** — save the current game state.
- **END ACTION / END TURN** — finish the current action or turn according to the current game state.

Card effects may request additional choices or input. These requests are presented through the appropriate GUI panel.

### AI

If the second player is configured as an AI player, the game starts the AI turn automatically. The AI evaluates available actions, fighters, destinations, targets, and cards through the `GameAI` component.

### Save / Load

The save system stores game state in JSON files. The load screen displays available saves and allows a saved game to be resumed.

---

# 🛠️ Technologies Used

| Technology | Purpose |
|------------|---------|
| C++17 | Programming language |
| CMake | Build system |
| SFML 3 | Graphics, windowing, input, and audio |
| nlohmann/json | JSON serialization for save/load |
| Git | Version control |
| GitHub | Source code hosting |

---

# 🎯 Programming Concepts

The project applies several important software-engineering concepts:

- Object-Oriented Programming
- Inheritance and polymorphism
- Encapsulation
- Separation of game logic and presentation
- Modular project structure
- Header/source separation
- STL containers and algorithms
- CMake-based builds
- JSON serialization
- Threading and synchronization for GUI/AI interaction
- Resource management through dedicated graphics and audio managers

The architecture keeps the core game state in the controller and domain classes while the `graphics` module handles rendering and user interaction.

---

# 🚀 Future Improvements

Possible future extensions include:

- Additional Heroes and Sidekicks
- Additional battlefields
- More complete AI strategies
- More card effects
- More animations and visual effects
- Expanded audio feedback
- Online multiplayer
- More save-management options
- Additional UI customization

---

# 👭 Contributing

Contributions are welcome.

If you would like to improve the project:

1. Fork the repository.
2. Create a new branch.

```bash
git checkout -b feature/YourFeature
```

3. Make your changes.
4. Build and test the project.
5. Commit your changes and open a pull request.

---

## ⭐ If you like this project

Give it a ⭐ on GitHub and help others discover it.
