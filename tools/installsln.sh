#!/bin/bash

# Define help message
print_help() {
  echo "Allowed options:"
  echo ""
  echo "required arguments:"
  echo "    --build-type <arg>      set target build type, pass this argument when running as a standalone command"
  echo "                            can be one of the following: Debug, Release, RelWithDebInfo, MinSizeRel"
  echo "    --build-dir <arg>       set build directory, pass this argument when running as a standalone command"
  echo ""
  echo "optional arguments:"
  echo "    -h [ --help ]           produce help message"
  echo "    --cmake-options <args>  define additional cmake options you want to pass to the build"
  echo "                            for a list of available options, see https://cmake.org/cmake/help/latest/manual/cmake.1.html#install-a-project"
}

# Parse command line arguments
cmake_options=""
found_cmake_options=false
while [[ $# -gt 0 ]]; do
  key="$1"

  case $key in
  -h | --help)
    print_help
    exit 0
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

# Check for valid build type
if [[ -z $build_type ]]; then
  echo "-- No build type specified. See -h, --help for more information."
  exit 33
else
  if [[ "$build_type" != "Debug" ]] && [[ "$build_type" != "Release" ]] && [[ "$build_type" != "RelWithDebInfo" ]] && [[ "$build_type" != "MinSizeRel" ]]; then
    echo "-- Invalid build type: $build_type"
    exit 33
  fi
  echo "-- Build type is set to: $build_type"
fi

# Set build directory
if [[ -z $build_dir ]]; then
  echo "-- No build directory specified. See -h, --help for more information."
  exit 33
else
  echo "-- Build directory is set to: $build_dir"
fi

# Run cmake build
cmd="cmake --install $build_dir/$build_type $cmake_options"
echo "$cmd"
eval "$cmd"
