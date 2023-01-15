<h1 align="center">OCXReader</h1>

OCXReader is a tool to read and parse data from OCX files and export them to
different formats such as STEP, SHIPXML, glTF, XCAF-XML or XCAF-XFB for
visualization.

![OCX Model](docu/napa-d-bulker.png)

## Roadmap

### Read OCX

In the long term, the OCXReader shall create a product structure like this is:

````shell
${vesselname}
|__ Reference Planes
|  |__ XRefPlanes
|  |  |__ Frame 1 ... Frame N
|  |__ YRefPlanes
|  |  |__ Longitudinal 1 ... Longitudinal N
|  |__ ZRefPlanes
|     |__ Deck 1 ... Deck N
|__ Reference Surfaces
|  |__ Ref Surface 1 ... Ref Surfaces N 
|__ Panels
   |__ Panel 1 ... Panel N
      |__ Contour
      |__ Surface
      |__ ComposedOf
      |  |__ Plate 1 ... Plate N
      |  |  |__ Contour
      |  |  |__ Surface
      |  |  |__ LimitedBy
      |  |     |__ Limit 1 ... Limit N
      |  |__ Bracket 1 ... Bracket N
      |  |__ Pillar 1 ... Pillar N
      |__ StiffenedBy
      |  |__ Stiffener 1 ... Stiffener N
      |__ LimitedBy
         |__ Limit 1 ... Limit N
````

Currently, major items like reference planes, panels, and plates are already
implemented. Stiffener traces are shown, but no stiffener representation.

### Supported Export Formats

- [x] STEP
- [ ] SHIPXML
- [ ] glTF
- [x] XCAF-XML
- [x] XCAF-XFB

## Setup

### Prerequisites

To build OCXReader you need to have the prerequisites set up:

#### Compiler

The project uses C++17 features and therefore requires a compiler that supports
this standard.

Compiler compatibility:

- Clang/LLVM >= 6
- MSVC++ >= 14.11 / Visual Studio >= 2017
- Xcode >= 10
- GCC >= 9
- MinGW >= 9

#### CMake

The project uses CMake as a build system. CMake version 3.24 or higher is
required.

#### vcpkg

This project is build with CMake and uses the
[vcpkg](https://vcpkg.io/en/index.html) package manager to install the required
dependencies. You can follow the steps to set up vcpkg on your
system [here](https://vcpkg.io/en/getting-started.html).

> Note: For WSL users, make sure to install vcpkg in a directory which grants
> permission for all users. Otherwise, you will get an error when trying to
> access and install vcpkg packages through a non-root user.

#### Python

The OpenCascade build requires python with a version of at least 3.7. However,
this is only needed when building on Linux.

To ensure your python3 points to the correct version run:

```shell
$ ls -l /usr/bin/python3
lrwxrwxrwx 1 root root 9  7. Jan 15:51 /usr/bin/python3 -> python3.7 # required python version >= 3.7
```

#### OpenCascade

The project uses OpenCascade as a geometry kernel. It is automatically
downloaded and build by `vcpkg` package manager.

To build OpenCascade on UNIX systems, you install the following packages:

<details><summary>Ubuntu</summary>
<p>

```shell
sudo apt-get install software-properties-common
sudo apt-get install libtool autoconf automake gfortran gdebi
sudo apt-get install gcc-multilib libxi-dev libxmu-dev libxmu-headers
sudo apt-get install libx11-dev mesa-common-dev libglu1-mesa-dev
sudo apt-get install libfontconfig1-dev
```

The minimum requirements for third party dependencies to run OpenCascade itself
is Freetype 2.5 and Tcl/TK 8.6:

```shell
sudo apt-get install libfreetype6 libfreetype6-dev
sudo apt-get install tcl tcl-dev tk tk-dev
```

</p>
</details>

<details><summary>openSUSE</summary>
<p>

> Note: The documentation is not complete yet. Feel free to contribute.

```shell
sudo zypper install libX11-devel Mesa-libHL-devel libXmu-devel libXi-devel
sudo zypper install bison fontconfig-devel 
```

The minimum requirements for third party dependencies to run OpenCascade itself
is Freetype 2.5 and Tcl/TK 8.6:

```shell
sudo apt-get install libfreetype6 libfreetype6-devel
sudo apt-get install tcl tcl-devel tk tk-devel
```

</p>
</details>

<details><summary>OSX</summary>
<p>

> Note: The documentation is not complete yet. Feel free to contribute.

</p>
</details>

### Build

OCXReader comes with a `cli` to make the setup and build step on the
command line as easy as possible. The project uses vcpkg to install the
dependencies, in most cases all you need to do is to provide the path to your
vcpkg installation directory.

A typical setup using the cli looks like this:

```shell
# Generate the build files (use cli.bat on Windows)
$ ./cli.sh gensln --vcpkg /path/to/vcpkg

# Build the project (use cli.bat on Windows)
$ ./cli.sh buildsln --build-type Release --build-dir ./build
```

> Note: See [Building on UNIX systems](#building-on-unix-systems) for more
> information on how to use the cli on UNIX systems.

The cli also provides functionality to combine the mentioned steps into one:

```shell
# Generate the build files and build the project (use cli.bat on Windows)
$ ./cli.sh gensln --vcpkg /path/to/vcpkg buildsln
#                                                ^ no need to specify --build-type and --build-dir as they get inherited from the previous gensln command
```

> Note: The cli is only recommended for use with the command line. When setting
> up the project in an IDE, you should use the IDE's functionality to generate
> the build files and build the project. To set up the project with vcpkg in an
> IDE, pass the `DCMAKE_TOOLCHAIN_FILE` variable to the CMake configuration
> options.

```shell
# CMake option to set the path to the vcpkg toolchain file
-DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

#### Building on UNIX systems

In order to build shared libraries on UNIX systems, vcpkg requires you to
provide a custom triplet file in the `DVCPKG_TARGET_TRIPLET` configuration
option. You can find custom, community provided triplets,
under `vcpkg/triplets/community`

To specify a custom triplet using the cli, make use of the `--cmake-options`
option in the `gensln` command:

```shell
# Generate the build files
$ ./cli.sh gensln --vcpkg /path/to/vcpkg --cmake-options -DVCPKG_TARGET_TRIPLET=x64-linux-dynamic
```

> Similarly pass the `DVCPKG_TARGET_TRIPLET` variable to the CMake configuration
> options when setting up the project in an IDE.

##### macOS

There is currently no official support in vcpkg to build the `opencascade`
library. This can be overridden by passing `--allow-unsupported`
to the `DVCPKG_INSTALL_OPTIONS` in the CMake options.

```shell
# Generate the build files
$ ./cli.sh gensln --vcpkg /path/to/vcpkg --cmake-options -DVCPKG_TARGET_TRIPLET=arm64-osx-dynamic -DVCPKG_INSTALL_OPTIONS=--allow-unsupported
```

#### Using an IDE

When setting up the project in an IDE, make sure to configure it by passing the
mentioned variables to the CMake configuration options:

##### CLion

Follow these steps to configure CMake options in CLion:

1. Open the project in CLion.
2. Go to File > Settings > Build, Execution, Deployment > CMake.
3. In the CMake options field, provide the necessary options to configure the
   project:

```shell
-DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=custom-triplet
```

4. Click Apply and then OK to save changes.

## Usage

OCXReader is a command line tool with the following options:

```shell
$ ocxreader --help
Allowed options:

Generic options:
  -v [ --version ]          print version string
  -h [ --help ]             produce help message
  --config-file arg         The path to the file containing OCX parsing options
                            (e.g. path/to/config_file.json)

OCXReader CLI options:
  -i [ --input-file ] arg   The OCX file to read
  --export-format arg       The export format(s) to use. This can be one or 
                            more of the following: STEP, SHIPXML, XCAF-XML, 
                            XCAF-XBF
  -s [ --save-to ] arg      The output-file path. Defines were to write the 
                            exported file(s) to. If not defined files get saved
                            relative to the program working directory.
  -o [ --output-file ] arg  The output file name. This is used as the filename 
                            to the defined export formats. If not defined 
                            input-file is used.
```

The generic option `--config-file` can be used to define the OCXReader CLI
options in a JSON file.
A sample configuration file can be found [here](ocxreader/config.example.json).

## Contributors

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tbody>
    <tr>
      <td align="center"><a href="https://github.com/skfcz"><img src="https://avatars.githubusercontent.com/u/1381962?v=4?s=64" width="64px;" alt="Carsten Zerbst"/><br /><sub><b>Carsten Zerbst</b></sub></a><br /><a href="https://github.com/skfcz/OCXReader/commits?author=skfcz" title="Code">💻</a> <a href="#maintenance-skfcz" title="Maintenance">🚧</a> <a href="https://github.com/skfcz/OCXReader/commits?author=skfcz" title="Documentation">📖</a> <a href="#ideas-skfcz" title="Ideas, Planning, & Feedback">🤔</a></td>
      <td align="center"><a href="https://github.com/paulbuechner"><img src="https://avatars.githubusercontent.com/u/45827409?s=400&u=a62152a15513e36652b045b5879f39f124120254&v=4?s=64" width="64px;" alt="Paul Büchner"/><br /><sub><b>Paul Büchner</b></sub></a><br /><a href="https://github.com/skfcz/OCXReader/commits?author=paulbuechner" title="Code">💻</a> <a href="#maintenance-paulbuechner" title="Maintenance">🚧</a> <a href="https://github.com/skfcz/OCXReader/commits?author=paulbuechner" title="Documentation">📖</a> <a href="#ideas-paulbuechner" title="Ideas, Planning, & Feedback">🤔</a></td>
    </tr>
  </tbody>
</table>

## License

This project falls under the [GNU LESSER GENERAL PUBLIC LICENSE v. 2.1](LICENSE)
