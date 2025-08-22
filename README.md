# Starship Game

<p align="center">
  <img src="thumbnail.png" alt="Starship Game Thumbnail" width="400"/>
</p>

A classic 2D space shooter game built with C++ and Qt.

## Features

- Player-controlled starship with movement and shooting
- Multiple enemy types and attack patterns
- Score tracking and high score table
- Power-ups (health, shield, etc.)
- Sound effects and background music
- Custom graphics and UI

## Getting Started

### Prerequisites

- Qt 5 or 6 (Qt Creator recommended)
- C++ compiler (GCC, Clang, or MSVC)

### Build Instructions

1. Open `Game.pro` in Qt Creator.
2. Configure the project for your environment.
3. Build and run the project.

Alternatively, from the command line:

```bash
qmake Game.pro
make
./Game
```

## Project Structure

- `main.cpp` / `mainwindow.*` — Main application and window logic
- `player.*`, `enemy.*`, `gun.*`, `score.*` — Core game classes
- `resources/` — Images, sounds, and other assets
- `resources.qrc` — Qt resource file

## Assets

All game graphics and sounds are located in the `resources/` folder.

## License

See [LICENCE.md](LICENCE.md) for license information.

## Credits

- Developed by [Your Name]
- Graphics and sounds by [Your Name or sources]
