# WinMarks

## Overview

**WinMarks** allows you to set hotkeys for specific windows on Mirosoft Windows
similar to the marks system in VIM

## Requirements

Visual c++ runtime XX

## Building

```powershell
mkdir build
cd build
# cmake ../
# 64 bit is currently not in a working state
cmake -A Win32 ..
cmake --build . --config Release
```

## Todo

* move from console application to system tray notification
* permanent marks that survive restart and can open an application that is not already running e.g. spotify
* GUI for manipulating hotkeys
* GUI for viewing / clearing marks
