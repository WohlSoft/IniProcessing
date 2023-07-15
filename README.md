# IniProcessing
A small INI file reader and writer which compatible with API of QSettings

It has no dependencies and using STL.

* Ready to use code you will find in `./src` folder
* Example of usage and unit tests you can find in the `./test` folder where you also can find a benchmark of this INI Procrssor and QSettings
* Use define the `INI_PROCESSING_ALLOW_QT_TYPES` macro to support QString and other Qt's types in the Qt applications

# How to run unit testing

You will need to have CMake>=3.2 and compiler with C++11 support
```bash
cmake .
make
make test
```
