# How to install SFML for development with VS Code
## Installing MSYS2 and mingw
> This is only for users who don't have those tools installed already.
1. Go to [MSYS2 Website](https://www.msys2.org/) and follow the installation instruction from step 1 to 5. Record the installation directory for later.
2. Launch MSYS2 and type in command `pacman -S --needed base-devel mingw-w64-x86_64-toolchain` press Enter, press Enter again when prompted to select modules to install, the type `y` and press Enter to confirm.
## Installing C/C++ extension for VS Code
> This is only for users who don't have the extension installed already.
1. Open VS Code and install the C/C++ extension via the extensions panel.
2. If prompted to select a compiler, choose g++ which will be located at msys64/mingw64/bin/g++.exe
## Creating project folder
1. Navigate to a folder of your choice (the path to that folder should not contain any non-ascii characters).
2. Create a folder for your project.
3. Create a folder named `.vscode` this will be where VS Code configuration files are stored.
4. Create a folder named `src` this will be where your source code is stored.
5. Create a folder named `bin` this will be where your binary (executable) files are stored.
6. Go to your MSYS2 installation directory and navigate to `mingw64/bin` then copy all .dll files to our bin folder in the project folder (not doing this may be fine if you added the `mingw64/bin` folder to path but will cause program to not run on other computers, unless you set everything to statically link which this guide does not intend to do)
## Downloading and installing SFML
1. Go to [SFML Download page](https://www.sfml-dev.org/download/sfml/2.6.0/) and download the one labeled `GCC 13.1.0 MinGW (SEH) - 64-bit`
2. Extract the archive.
3. Copy the extracted folder to your project folder.
4. Go into the SFML folder and copy all dlls in the bin directory to our bin folder in the project folder.
## Example Folder Structure
Here is an example of what your project folder should looks like once you finish this guide. Don't worry if your doesn't match exactly, this is just to give you an idea of what we're aiming for and to help you put things in the right places.

 ![Example folder structure image](https://raw.githubusercontent.com/Nat-thapas/Minecraft-SFML-Edition/master/exampleFolderStructure.png) 

The dll files are in the pink bin folder along with `test.exe`, they're just hidden.
## Configuring VS Code
1. In the .vscode folder, create a file named `launch.json` and configure how the debugger will be launched. Here is an example that should work if you installed mingw and MSYS2 via the instructions above.
```
{
    "configurations": [
        {
            "name": "C/C++: g++.exe build and debug active file",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}\\bin\\${fileBasenameNoExtension}.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}\\bin",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "C:\\msys64\\mingw64\\bin\\gdb.exe",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                },
                {
                    "description": "Set Disassembly Flavor to Intel",
                    "text": "-gdb-set disassembly-flavor intel",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "C/C++: g++.exe build active file for debug"
        }
    ],
    "version": "2.0.0"
}
```
2. In the .vscode folder, create a file named `tasks.json` and configure how your code will be compiled. Here is an example that should work if you installed mingw and MSYS2 via the instructions above.
```
{
    "tasks": [
        {
            "type": "cppbuild",
            "label": "C/C++: g++.exe build active file for debug",
            "command": "C:\\msys64\\mingw64\\bin\\g++.exe",
            "args": [
                "-fdiagnostics-color=always",
                "--std=gnu++20",
                "-O0",
                "-g",
                "-Wall",
                "-Wextra",
                "-Wpedantic",
                "${file}",
                "-o",
                "${workspaceFolder}\\bin\\${fileBasenameNoExtension}.exe",
                "-I${workspaceFolder}\\SFML-2.6.0\\include",
                "-L${workspaceFolder}\\SFML-2.6.0\\lib",
                "-lsfml-graphics-d",
                "-lsfml-window-d",
                "-lsfml-audio-d",
                "-lsfml-network-d",
                "-lsfml-system-d"
            ],
            "options": {
                "cwd": "${fileDirname}"
            },
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "detail": "Use this task to build the file for debugging"
        },
        {
            "type": "cppbuild",
            "label": "C/C++: g++.exe build active file for release",
            "command": "C:\\msys64\\mingw64\\bin\\g++.exe",
            "args": [
                "-fdiagnostics-color=always",
                "--std=gnu++20",
                "-O2",
                "-s",
                "-DNDEBUG",
                "-Wall",
                "-Wextra",
                "-Wpedantic",
                "${file}",
                "-o",
                "${workspaceFolder}\\bin\\${fileBasenameNoExtension}.exe",
                "-I${workspaceFolder}\\SFML-2.6.0\\include",
                "-L${workspaceFolder}\\SFML-2.6.0\\lib",
                "-lsfml-graphics",
                "-lsfml-window",
                "-lsfml-audio",
                "-lsfml-network",
                "-lsfml-system"
            ],
            "options": {
                "cwd": "${fileDirname}"
            },
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": false
            },
            "detail": "Use this task to build the file for release"
        }
    ],
    "version": "2.0.0"
}
```
## Testing / usage
1. Open VS Code in the project folder, the code you write (.cpp, .hpp files) will be in the `src` directory and the executables compiled from those code will be in the `bin` directory.
2. Create a file in `src` directory named whatever you want.
3. Open the file you've just created and copy the following example code the the file
```
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}
```
4. Click the launch button (the play button on the top left of VS Code). If everything works you should see a green circle on your screen.
5. To add resources such as textures or fonts to your game, copy it to the bin folder and load it with relative path from your code. For example if you have `zombie.png` texture in the bin folder, to load it you simply do `texture.loadFromFile('zombie.png')`
6. If something doesn't work then try reading the error message and figure out what's wrong, if that doesn't work then ask someone to figure it out for you. If that still doesn't work then either ask someone else or give up.
## Extra configurations
1. For convenient, you can create a file named `c_cpp_properties.json` in the .vscode folder and add the following to switch C++ standard to C++20, which allows you to use std::format
```
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**"
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE"
            ],
            "compilerPath": "C:\\msys64\\mingw64\\bin\\g++.exe",
            "cStandard": "c17",
            "cppStandard": "gnu++20",
            "intelliSenseMode": "windows-gcc-x64"
        }
    ],
    "version": 4
}
```
2. For more convenient, you can create a file named `settings.json` in the .vscode folder and add the following to hide .dll files in the bin folder and exclude SFML folder from search results
```
{
    "search.exclude": {
        "**/SFML-2.6.0": true
    },
    "files.exclude": {
        "**/*.dll": true
    }
}