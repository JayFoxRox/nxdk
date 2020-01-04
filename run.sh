#!/bin/env bash

FAILURE='\033[0;31m'
SUCCESS='\033[0;32m'
HEADLINE='\033[1m'
COMMENT='\033[0;37m'
RESET='\033[0m'

if [[ -z "${NXDK_DIR}" ]]; then
  NXDK_DIR="."
  echo -e "${COMMENT}NXDK_DIR not set, using NXDK_DIR=\"${NXDK_DIR}\"${RESET}"
  echo ""
fi

fp20compiler="${NXDK_DIR}/tools/fp20compiler/fp20compiler"
if [[ ! -f "${fp20compiler}" ]]; then
  echo -e "${FAILURE}Unable to find fp20compiler at \"${fp20compiler}\"${RESET}"  
  exit 1
else
  if [[ ! -x "${fp20compiler}" ]]; then
    echo -e "${FAILURE}fp20compiler found at \"${fp20compiler}\", but not executable${RESET}"  
    exit 1
  fi
fi

function process_file() {
  path=$1

  echo ""
  echo -e "${HEADLINE}${path}${RESET}"
  echo -e "${COMMENT}"
  cat -n "${path}"
  echo -e "${RESET}"
  "${fp20compiler}" "${path}" > /dev/null
  status=$?
  #valgrind "${fp20compiler}" "$path" > /dev/null
  if [ $status -eq 0 ]; then
    echo -e "${SUCCESS}Success${RESET}"
  else
    echo -e "${FAILURE}Failure: ${status}${RESET}"
  fi
  echo ""
}

function process_path() {
  path=$1

  # Process files
  for i in $path/*; do
    if [[ -f "$i" ]]; then
      if [[ "$i" =~ \.ps.asm$ ]]; then
        process_file "${i}"
      else
        echo -e "${COMMENT}Skipping unrecognized \"${i}\"${RESET}"
      fi
    fi
  done

  # Process subdirectories (avoiding symlinks)
  for i in $path/*; do
    if [[ -d "${i}" && ! -L "${i}" ]]; then
      process_path "${i}"
    fi
  done
    
}

path=$1
if [[ -z "${path}" ]]; then
  path="."
fi
process_path "${path}"
