

* https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-powershell

* Make sure to use the MSVC 2022 Developer Console and not vanilla powershell... otherwise you run into issues of package dependencies not being found by cmake!

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


# Helloworld example
Run using MSVC 2022 compiler on windows

```bash
# generates a vcpkg.json file containing the dependencies installed
vcpkg new --application

# adds the fmt library to vcpkg.json
vcpkg add port fmt

# tells cmake to configure the build with vcpkg linked as a dependency to manage packages
cmake --preset=default

# tells cmake to compile the c++ project using the compiler and settings its configured with from the previous step
# build gets stored in the build folder on success
cmake --build build

# run the app
.\build\HelloWorld.exe
```