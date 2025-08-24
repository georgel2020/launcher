# Launcher

*A blazing fast, keyboard-driven launcher for Windows, built with C++ and Qt.*

## Usage

- Press Alt + Space to bring up the search bar
- Type your query to see the results
- Use the up / down arrow keys to navigate the result list
- Press Enter to execute the default action for the selected item
- Press Tab or left / right arrow keys to select other available actions for the highlighted item (if any)
- Press a specific shortcut (e.g., Ctrl + Shift + Enter) to execute a specific action (e.g., run as administrator)

## Features

Launcher offers a set of built-in modules:

### Apps Search

Find and launch applications installed on your system.

Configuration:

```json
{
  "apps": [
    {
      "keywords": [
        "yourapp",
        "anothername"
      ],
      "name": "Your App",
      "path": "C:\\Program Files\\YourApp\\YourApp.exe"
    }
  ]
}
```

On the first run, a default configuration file will be generated based on Start Menu items.

### Calculator

Perform mathematical calculations directly in the search bar. This feature is based
on [muparser](https://github.com/beltoforion/muparser).

For example:

- `(265+239)/7`
- `sin(_pi/6)`
- `sqrt(3)`

### Everything Search

Provides instant full-disk file and folder searches with [Everything](https://www.voidtools.com/). Everything must be
running in the background for this feature to work.

Configuration:

```json
{
  "maxResults": 50,
  // Max number of results to show.
  "runCountWeight": 1
  // Weight of the run count in the search results. Set to 0 to disable.
}
```

### Windows Terminal

Search for Windows Terminal profiles.

### System Commands

Execute system-level actions.

Supported commands:

- `shutdown`
- `restart`
- `lock`

### Unit Converter

Perform common unit conversions. This feature is based on [units](https://github.com/LLNL/units).

For example:

- `3m/s in km/h`
- `100 floz to ml`

### Launcher Commands

Core commands to control Launcher itself.

- `exit`: Exit Launcher
- `configure`: Open Launcher configuration location
- `version`: Show Launcher version

## Configuration

Default configuration files will be generated in `%APPDATA%\Launcher\` on the first run.

`Launcher.json`

```json
{
  "history": {
    "decay": 0.95,
    // After each day, the scores of previously launched results are multiplied by this value.
    "historyScoreWeight": 1,
    // Weight of the history score in the search results. Set to 0 to disable.
    "increment": 1,
    // After each launch, the score of the result is incremented by this value.
    "minScore": 0.01
    // If the score of a result is lower than this value, it will be removed from history.
  },
  "modules": {
    "moduleName": {
      "enabled": true,
      "global": true,
      // If false, the results will only be shown when the search query starts with the assigned prefix.
      "prefix": " ",
      // Prefix to use for the module. Set to ' ' to disable.
      "priority": 1
    }
  },
  "ui": {
    "maxVisibleResults": 5
    // Max number of results shown before scrolling.
  }
}
```

### Theme

`Theme.json`

```json
{
  "colors": {
    "dark": {
      "background": {
        "accent": "#82d5c7",
        "active": "#334b47",
        "default": "#252b2a"
      },
      "text": {
        "accent": "#003731",
        "default": "#dde4e1"
      }
    },
    "light": {
      "background": {
        "accent": "#006b60",
        "active": "#cae6df",
        "default": "#e3eae7"
      },
      "text": {
        "accent": "#ffffff",
        "default": "#171d1b"
      }
    }
  },
  "theme": "auto",
  // "auto" to automatically detect the system theme; "dark" or "light" to force a specific theme.
}
```
