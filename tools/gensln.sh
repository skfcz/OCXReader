#!/bin/bash

# Define help message
print_help() {
  echo "Allowed options:"
  echo ""
  echo "required arguments:"
  echo "    --vcpkg <path>          the path to the vcpkg directory (e.g. C:/dev/vcpkg)"
  echo ""
  echo "optional arguments:"
  echo "    -h [ --help ]           produce help message"
  echo "    --build-type <arg>      manually set target build type (default: Release)"
  echo "                            can be one of the following: Debug, Release, RelWithDebInfo, MinSizeRel"
  echo "    --build-dir <arg>       manually set build directory (default: build)"
  echo "    --cmake-options <args>  define additional cmake options you want to pass to the project generation"
  echo "                            for a list of available options, see https://cmake.org/cmake/help/latest/manual/cmake.1.html#generate-a-project-buildsystem"
}

# Parse command line arguments
found_cmake_options=false
while [[ $# -gt 0 ]]; do
  key="$1"

  case $key in
  -h | --help)
    print_help
    exit 0
    ;;
  --vcpkg)
    vcpkg_dir="$2"
    shift # past argument
    shift # past value
    ;;
  --build-type)
    build_type="$2"
    shift # past argument
    shift # past value
    ;;
  --build-dir)
    build_dir="$2"
    shift # past argument
    shift # past value
    ;;
  --cmake-options)
    found_cmake_options=true
    shift # past argument
    ;;
  *) # unknown option
    # if we found the --cmake-options argument, we assume that all the remaining arguments are cmake options
    if [[ "$found_cmake_options" = true ]]; then
      cmake_options="$cmake_options $key"
      shift # past key
    else
      echo "Unknown option: $key"
      print_help
      exit 33
    fi
    ;;
  esac
done

# Check if all the required arguments have been passed
missing_arguments="Missing arguments. See -h, --help for more information."
echo "-- Checking arguments"
if [[ -z $vcpkg_dir ]]; then
  echo $missing_arguments
  exit 33
else
  # Check if vcpkg directory exists
  if [ ! -f "$vcpkg_dir/scripts/buildsystems/vcpkg.cmake" ]; then
    echo "-- Cannot find vcpkg.cmake in $vcpkg_dir/scripts/buildsystems/"
    exit 33
  else
    echo "-- vcpkg directory is set to: $vcpkg_dir"
  fi
fi

# Check for valid build type
if [[ -z $build_type ]]; then
  build_type="Release"
  echo "-- No build type specified. Defaulting to: $build_type"
else
  if [[ "$build_type" != "Debug" ]] && [[ "$build_type" != "Release" ]] && [[ "$build_type" != "RelWithDebInfo" ]] && [[ "$build_type" != "MinSizeRel" ]]; then
    echo "-- Invalid build type: $build_type"
    exit 33
  fi
  echo "-- Build type is set to: $build_type"
fi

# Set build directory
if [[ -z $build_dir ]]; then
  build_dir="./build"
  echo "-- No build directory specified. Defaulting to: $build_dir"
else
  echo "-- Build directory is set to: $build_dir"
fi

echo "-- Checking arguments: done"

# Generate project
cmd="cmake -S . -B $build_dir/$build_type -DCMAKE_BUILD_TYPE=$build_type -DCMAKE_TOOLCHAIN_FILE=$vcpkg_dir/scripts/buildsystems/vcpkg.cmake $cmake_options"
echo $cmd
eval $cmd
