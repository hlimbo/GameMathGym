# Install Visual Studio 2022 MSCVC 2022 Developer console
* To install the MSVC 2022 Developer console, follow this link:
* https://learn.microsoft.com/en-us/visualstudio/releases/2022/release-history#evergreen-bootstrappers
* Under Evergreen bootstrappers, install the community version
![alt text](image.png)
* Follow the on screen instructions of the installer and ensure `MSVC 2022 Developer console` is included as part of the installation process


# How to install vcpkg on Windows?
* vcpkg is a C++ package manager that allows you to install 3rd party libraries for your C++ project
* Follow the steps outlined in the link below to install vcpkg:
* https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-powershell
* **NOTE:** installation is via manifest mode through the `vcpkg.json` file

# How to let CMake know where your vcpkg installation is located?
* create a `CMakeUserPresets.json` file in the root of this project
* Copy / Paste this template into `CMakeUserPresets.json`
* Replace `<path to vcpkg>` with the **absolute path** to your vcpkg installation folder path

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "desktop-local",
      "inherits": "desktop",
      "environment": {
        "VCPKG_ROOT": "<absolute-path-to-vcpkg-folder>"
      }
    },
    {
      "name": "web-local",
      "inherits": "web",
      "environment": {
        "VCPKG_ROOT": "<absolute-path-to-vcpkg-folder>",
        "EMSDK": "<absolute-path-to-emsdk-folder>",
        "Path": "<absolute-path-to-folder-containing-embedded-emsdk-python-executable>;$penv{Path}"
      }
    }
  ]
}
```

**NOTE:** For the `Path` environment variable, this will be the absolute path to the folder containing the embedded emsdk python executable. For example, the absolute path could be `C:\\Cpp\\emsdk\\python\\3.13.3_64bit` on Windows machines. This fixes an error where the embedded emsdk python version cannot be found where cmake stops the configuration step prematurely.


How to build project using cmake?
```powershell
# Configures using desktop preset
cmake --preset desktop

# Builds using desktop preset
cmake --build --preset desktop-build --clean-first

# Configures using web preset
cmake --preset web-local

# Builds using web preset
cmake --build --preset web-build --clean-first
```

What does the --build command do in cmake?
* builds the artifacts in the build folder. Building involves running a c++ compiler to compile the code, link library dependencies, include `.dll` files required as dependencies, and ensure header files are included in order for a successful compilation. `--clean-first` command deletes previously built object files, libraries, and executables before starting a new build.

How to clear build folder for fresh install?
* delete the `build` folder from your machine by opening windows explorer and moving the `build` folder to the recycling bin.

Powershell commands usage example for a hello world app
```bash
# generates a vcpkg.json file containing the dependencies installed
vcpkg new --application

# adds the fmt library to vcpkg.json in manifest mode
vcpkg add port fmt

# tells cmake to configure the build with vcpkg linked as a dependency to manage packages
cmake --preset=default

# tells cmake to compile the c++ project using the compiler and settings its configured with from the previous step
# build gets stored in the build folder on success
cmake --build build

# run the app
.\build\HelloWorld.exe
```

How to run the emscripten build on a web browser?
* After you finish running the configuration and build steps in cmake for the web build, do the following steps:
```powershell
# You should have GameMathGym.html, GameMathGym.js, and GameMathGym.wasm compiled successfully by emscripten in the folder you are going to cd into
cd build/web
# start the python server -- access project http://localhost:8000/GameMathGym.html
python -m http.server 8000
```