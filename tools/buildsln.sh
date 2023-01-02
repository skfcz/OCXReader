#!/bin/bash

# Define help message
print_help() {
  echo "Allowed options:"
  echo ""
  echo "required arguments:"
  echo "    --build-type <arg>      set target build type, pass this argument when running as a standalone command (without gensln)"
  echo "                            can be one of the following: Debug, Release, RelWithDebInfo, MinSizeRel"
  echo "    --build-dir <arg>       set build directory, pass this argument when running as a standalone command (without gensln)"
  echo ""
  echo "optional arguments:"
  echo "    -h [ --help ]           produce help message"
  echo "    <args>                  define additional cmake options you want to pass to the build"
  echo "                            for a list of available options, see https://cmake.org/cmake/help/latest/manual/cmake.1.html#build-a-project"
}

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
  *) # unknown option
    rest="$rest $key"
    shift # past key
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
cmd="cmake --build ./$build_dir/$build_type --target ocxreader $rest"
echo $cmd
eval $cmd
