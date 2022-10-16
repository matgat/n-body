## [n-body](https://github.com/matgat/n-body.git)

A fun project to perform N-body simulation.


_________________________________________________________________________
## Build
```
# pacman -S fmt sfml
$ git clone https://github.com/matgat/n-body.git
$ cd n-body
$ clang++ -std=c++2b -O3 -Wall -Wextra -Wpedantic -Wconversion -Wfatal-errors -lfmt -lsfml-graphics -lsfml-window -lsfml-system "n-body.cpp" -o "n-body"
```
On windows, using Visual Studio, the `sfml` library
dependency can be conveniently handled with `vcpkg`:
```
> git clone https://github.com/Microsoft/vcpkg.git
> .\vcpkg\bootstrap-vcpkg.bat -disableMetrics
> .\vcpkg\vcpkg integrate install
> .\vcpkg\vcpkg install fmt:x64-windows
```
