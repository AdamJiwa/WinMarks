# WinMarks

## Overview

**WinMarks** allows you to set hotkeys for specific windows on Mirosoft Windows
similar to the marks system in VIM

## Building

# Requirements
Wix toolchain

For repackaging icons currently using imageMagick
```powershell
magick convert .\Icon_16x16.png .\Icon_32x32.png .\Icon_48x48.png .\Icon_60x60.png -colors 256 Icon.ico
```

```powershell
mkdir build
cd build
# cmake ../
# 64 bit is currently not in a working state
cmake -A Win32 ..
# To generate installer with build append "--target package"
cmake --build . --config Release
# To generate installer seperately
cpack .
```

## Todo

* Find a nice way to setup transaltions for wix strings with cpack
* permanent marks that survive restart and can open an application that is not already running e.g. spotify
* GUI for manipulating hotkeys
* GUI for viewing / clearing marks
