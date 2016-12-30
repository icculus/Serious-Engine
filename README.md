# Serious Engine
========================

This is the source code for Serious Engine v.1.10, including the following projects:

* `DedicatedServer`
* `Ecc` The *Entity Class Compiler*, a custom build tool used to compile *.es files
* `Engine` Serious Engine 1.10
* `EngineGUI` Common GUI things for game tools
* `EntitiesMP` All the entity logic
* `GameGUIMP` Common GUI things for game tools
* `GameMP` All the game logic
* `Modeler` Serious Modeler
* `RCon` Used to connect to servers using an admin password
* `SeriousSam` The main game executable
* `SeriousSkaStudio` Serious Ska Studio
* `WorldEditor` Serious Editor
* `DecodeReport` Used to decode crash *.rpt files
* `Depend` Used to build a list of dependency files based on a list of root files
* `LWSkaExporter` Exporter for use in LightWave
* `MakeFONT` Used for generating *.fnt files
* `Shaders` Compiled shaders
* `GameAgent` The serverlist masterserver written in Python
* `libogg`, `libvorbis` Third party libraries used for playing OGG-encoded ingame music (see http://www.vorbis.com/ for more information)

These have been modified to run correctly under the recent version of Windows. (Tested: Win7 x64, Win8 x64, Win8.1 x64)

Building
--------

### Windows

To build Serious Engine 1, you'll need Visual Studio 2013 or 2015, Professional or Community edition ( https://www.visualstudio.com/post-download-vs?sku=community ).

Do not use spaces in the path to the solution.

Once you've installed Visual Studio and (optionally) DirectX8 SDK, you can build the engine solution (`/Sources/All.sln`). Press F7 or Build -> Build solution. The libraries and executables will be put into `\Bin\` directory (or `\Bin\Debug\` if you are using the Debug configuration).

### Linux

The following instructions assume that you are executing the terminal commands with the git repository as the current working directory (`pwd`).

#### Setting up the repository

Type this in your terminal:

```
git clone https://github.com/rcgordon/Serious-Engine.git
cd Serious-Engine
```

#### Copy official game data (optional)

If you have access to a copy of the game (either by CD or through Steam),
you can copy the \*.gro files from the game directory to the root directory
of the repository (**not** in `Bin/`!).


**For TFE:** Other than copying the .gro file, you also need to copy the
contents of the `Levels/` directory from your CD/Steam into the repository's
`Levels/` directory.

#### Building (only for SS:TSE)

Type this in your terminal:

```
Sources/build-linux64.sh            # use build-linux32.sh for 32-bits
cp Sources/cmake-build/ssam Bin/
cp Sources/cmake-build/Debug/* Bin/Debug/
```

#### Building (only for SS:TFE)

Same as SS:SE, but note the following:

- Before running build-linux64.sh, modify the file by passing `-DTFE=TRUE` to cmake.
- After building, you need to copy 'ssam**-tfe**' instead of 'ssam', as shown:

  ```
  cp Sources/cmake-build/ssam-tfe Bin/
  ```
  
#### ModEXT.txt

* For TFE: Ensure that ModEXT.txt is empty, or is deleted.
* For TSE: Ensure that ModEXT.txt contains the text 'MP' (without the quotes). By default, this repository already have such a file.

#### Running

Type this in your terminal:

```
Bin/ssam
```

(or `ssam-tfe` if you are running TFE)

Optional features
-----------------

DirectX support is disabled by default. If you need DirectX support you'll have to download DirectX8 SDK (headers & libraries) ( http://files.seriouszone.com/download.php?fileid=759 or https://www.microsoft.com/en-us/download/details.aspx?id=6812 ) and then enable the SE1_D3D switch for all projects in the solution (Project properties -> Configuration properties -> C/C++ -> Preprocessor -> Preprocessor definitions -> Add "SE1_D3D" for both Debug and Release builds). You will also need to make sure the DirectX8 headers and libraries are located in the following folders (make the folder structure if it's not existing yet):
* `/Tools.Win32/Libraries/DX8SDK/Include/..`
* `/Tools.Win32/Libraries/DX8SDK/Lib/..`

MP3 playback is disabled by default. If you need this feature, you will have to copy amp11lib.dll to the '\Bin\' directory (and '\Bin\Debug\' for MP3 support in debug mode). The amp11lib.dll is distributed with older versions of Serious Sam: The First Encounter.

3D Exploration support is disabled in the open source version of Serious Engine 1 due to copyright issues. In case if you need to create new models you will have to either use editing tools from any of the original games, or write your own code for 3D object import/export.

IFeel support is disabled in the open source version of Serious Engine 1 due to copyright issues. In case if you need IFeel support you will have to copy IFC22.dll and ImmWrapper.dll from the original game into the `\Bin\` folder.

Running
-------

This version of the engine comes with a set of resources (`\SE1_10.GRO`) that allow you to freely use the engine without any additional resources required. However if you want to open or modify levels from Serious Sam Classic: The First Encounter or The Second Encounter (including most user-made levels), you will have to copy the game's resources (.GRO files) into the engine folder. You can buy the original games on Steam, as a part of a bundle with Serious Sam Revolution ( http://store.steampowered.com/app/227780 )

When running a selected project, make sure its project settings on Debugging is set to the right command:
* For debug:
    $(SolutionDir)..\Bin\Debug\$(TargetName).exe`
* For release:
    $(SolutionDir)..\Bin\$(TargetName).exe`
And its working directory:
    $(SolutionDir)..\

Common problems
---------------

Before starting the build process, make sure you have a "Temp" folder in your development directory. If it doesn't exist, create it.
SeriousSkaStudio has some issues with MFC windows that can prevent the main window from being displayed properly.

License
-------

Serious Engine is licensed under the GNU GPL v2 (see LICENSE file).

Some of the code included with the engine sources is not licensed under the GNU GPL v2:

* zlib (located in `Sources/Engine/zlib`) by Jean-loup Gailly and Mark Adler
* LightWave SDK (located in `Sources/LWSkaExporter/SDK`) by NewTek Inc.
* libogg/libvorbis (located in `Sources/libogg` and `Sources/libvorbis`) by Xiph.Org Foundation
