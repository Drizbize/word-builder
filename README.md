# WordBuilder

A small game in C++23 with [raylib](https://www.raylib.com): the word you have to build follows your mouse, letters fly across the screen, and you must hit the right letters in the right order. A wrong letter costs a life; completing a word moves you on to the next one, and the difficulty can change from word to word.

> Personal project. The level texts in this repository are placeholder words, replace them with your own.

## Features
- Levels described in JSON (see below): lives, speed, spawn delay, letter movement (linear, sine wave, curve), rotation and camera rotation
- Several ways to reveal a word: all letters, row by row, random; next letter on the right, on the left, from both sides or random; visible, invisible or "exploring" style
- Level menu, score and progress saved to `user_save.json`

## Build and run
Requirements: CMake 3.25+, a C++23 compiler, and the system libraries raylib needs on Linux (X11 or Wayland, OpenGL).

```bash
git clone --recurse-submodules <this repository>
cd word-builder
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
cd build && ./wordBuilder
```

The level files are copied next to the executable at configure time, and the game loads them from its working directory, so start it from `build/`. Click a level in the menu to play it; press `E` to leave a level.

## Level files
`levels.json` lists the levels; each entry points to a file in `levels/`. A level file contains:

```json
{
  "lifes": 5,
  "first_text": "Text shown first",
  "text_layers": [
    [
      { "difficulty": { "timerOut": 2.5, "vMin": 50.0, "vMax": 75.0 },
        "strategy": { "init": "RND", "method": "RND_SIDE", "style": "INVISIBLE" } },
      "Test",
      "word"
    ]
  ]
}
```

Words are strings; the `difficulty` and `strategy` objects between them apply to the words that follow. `levels/example.json` shows every option (`vRot`, `cameraRot`, `moveType`: `LINEAR`, `SINE_WAVE`, `CURVE`).

## Dependencies
| Library | Source |
|---------|--------|
| raylib 6.0 | git submodule `libs/raylib` |
| raylib-cpp 6.0.3 | git submodule `libs/raylib-cpp` |
| rlText (text rendering, zlib licence) | copy in `libs/rlTextLib` |
| nlohmann/json 3.12.0 (MIT) | `libs/json` |
