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

# How to let CMake know where your vcpkg installation is located?
* create a `CMakeUserPresets.json` file in the root of this project
* Copy / Paste this template into `CMakeUserPresets.json`
* Replace `<path to vcpkg>` with the **absolute path** to your vcpkg installation folder path

```json
{
  "version": 2,
  "configurePresets": [
    {
      "name": "default",
      "inherits": "vcpkg",
      "environment": {
        "VCPKG_ROOT": "<path to vcpkg>"
      }
    }
  ]
}
```

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