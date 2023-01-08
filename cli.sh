#!/bin/bash

# Define help message
print_help() {
  echo "Allowed options:"
  echo ""
  echo "    -h [ --help ]     produce help message"
  echo "    gensln <args>     generate a solution. For a list of available options, see gensln --help"
  echo "    buildsln <args>   build a solution. For a list of available options, see buildsln --help"
}

# Parse command line arguments
found_gensln=false
found_gensln_parse_help=false

found_buildsln=false
found_buildsln_parse_help=false

found_run=false
while [[ $# -gt 0 ]]; do
  key="$1"

  case $key in
  -h | --help)
    if [[ "$found_gensln" = true ]]; then
      gensln_options="$gensln_options $key"
      shift # past key
    elif [[ "$found_buildsln" = true ]]; then
      buildsln_options="$buildsln_options $key"
      shift # past key
    else
      print_help
      exit 0
    fi
    ;;
  gensln)
    found_gensln=true
    found_gensln_parse_help=true
    shift # past argument
    ;;
  buildsln)
    found_buildsln=true
    found_buildsln_parse_help=true
    found_gensln_parse_help=false
    shift # past argument
    ;;
  run)
    found_run=true
    found_gensln_parse_help=false
    found_buildsln_parse_help=false
    shift # past argument
    ;;
  *) # unknown option
    if [[ "$found_gensln_parse_help" = true ]]; then
      gensln_options="$gensln_options $key"
      shift # past key
    elif [[ "$found_buildsln_parse_help" = true ]]; then
      buildsln_options="$buildsln_options $key"
      shift # past key
    elif [[ "$found_run" = true ]]; then
      run_options="$run_options $key"
      shift # past key
    else
      echo "Unknown option: $key"
      echo "Usage: $0 [gensln|buildsln|run] [options]"
      exit 33
    fi
    ;;
  esac
done

# Run gensln
if [[ $found_gensln = true ]]; then
  echo "-- Running gensln with options: $gensln_options"
  . ./tools/gensln.sh $gensln_options
  if [[ $? -ne 0 ]]; then
    echo "-- Failed to generate solution"
    exit 33
  fi
fi

# Run buildsln
if [[ $found_buildsln = true ]]; then
  echo "-- Running buildsln with options: $buildsln_options"
  . ./tools/buildsln.sh $buildsln_options
  if [[ $? -ne 0 ]]; then
    echo "-- Failed to build solution"
    exit 33
  fi
fi

# Run run
if [[ $found_run = true ]]; then
  echo "-- Running run with options: $run_options"
  ./tools/run.sh $run_options
  if [[ $? -ne 0 ]]; then
    echo "-- Failed to run solution"
    exit 33
  fi
fi
