## [n-body](https://github.com/matgat/n-body.git)

A fun project to perform N-body simulation.


_________________________________________________________________________
## Build
You need a `c++20` compliant toolchain.

```sh
$ git clone https://github.com/matgat/n-body.git
$ python n-body/build/build.py
```

### linux
Launch `make` directly:

```sh
$ cd n-body/build
$ make
```

To run unit tests:

```sh
$ make test
```

> [!TIP]
> Install the dependency `sfml` using
> your package manager:
>
> ```sh
> $ sudo pacman -S sfml
> ```
>
> or
>
> ```sh
> $ sudo apt install -y libsfml-dev
> ```


### Windows

On Windows you need Microsoft Visual Studio 2022 (Community Edition).
Once you have `msbuild` visible in path, you can launch the build from the command line:

```bat
> msbuild build/n-body.vcxproj -t:Rebuild -p:Configuration=Release -p:Platform=x64
```

> [!TIP]
> Install the dependency `sfml` using `vcpkg`:
>
> ```bat
> > git clone https://github.com/Microsoft/vcpkg.git
> > cd .\vcpkg
> > .\bootstrap-vcpkg.bat -disableMetrics
> > .\vcpkg integrate install
> > .\vcpkg install sfml:x64-windows
> ```
>
> To just update the `vcpkg` libraries:
>
> ```bat
> > cd .\vcpkg
> > git pull
> > .\bootstrap-vcpkg.bat -disableMetrics
> > .\vcpkg upgrade --no-dry-run
> ```
