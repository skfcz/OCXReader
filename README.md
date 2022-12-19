<h1 align="center">OCXReader</h1>

OCXReader is a tool to read and parse data from OCX files and export them to
different formats such as STEP, SHIPXML, glTF, XCAF-XML or XCAF-XFB for
visualization.

## Roadmap

### Read OCX

In the long term a product structure like this is planned:

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

### Export

- [x] STEP
- [ ] SHIPXML
- [ ] glTF
- [x] XCAF-XML
- [x] XCAF-XFB

## Setup

### Prerequisites

#### vcpkg

This project is build with CMake and uses the
[vcpkg](https://vcpkg.io/en/index.html) package manager to install the required
dependencies. You can follow the steps to set up vcpkg on your
system [here](https://vcpkg.io/en/getting-started.html).

> Note: For WSL users, make sure to install vcpkg in a directory which grants
> permission for all users. Otherwise, you will get an error when trying to
> access and install vcpkg packages through a non-root user.

### Build

The project uses vcpkg to install the dependencies, in most cases you need to
provide the path to the vcpkg installation directory. This can be done by
passing `DCMAKE_TOOLCHAIN_FILE` in your CMake options.

#### Windows

```shell
# CMake options
cmake -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

#### Linux

Before you can build the project you need to install the following third party
dependencies to build OpenCascade:

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

In order to build shared libraries on non-Windows systems, vcpkg requires you to
provide a custom triplet file in `DVCPKG_TARGET_TRIPLET` configuration option.
You can find community provided triplet files under `vcpkg/triplets/community`
directory.

```shell
# CMake options
cmake -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-linux-dynamic
```

#### OSX

In order to build shared libraries on non-Windows systems, vcpkg requires you to
provide a custom triplet file in `DVCPKG_TARGET_TRIPLET` configuration option.
You can find community provided triplet files under `vcpkg/triplets/community`
directory.

> For building on OSX, vcpkg currently has no support official for building
> `opencascade` library. This can be overridden by passing `--allow-unsupported`
> to the `DVCPKG_INSTALL_OPTIONS` in the CMake options.

```shell
# CMake options
cmake -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-osx-dynamic -DVCPKG_INSTALL_OPTIONS=--allow-unsupported
```

## Usage

OCXReader should be used as a command line tool. The following options are
available:

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
