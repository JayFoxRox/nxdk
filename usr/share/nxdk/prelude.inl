set -u
set -e

# Add nxdk tools to path
PATH="${PATH}:${NXDK_DIR}/usr/bin:${NXDK_DIR}/usr/local/bin"

# Find platform information
UNAME_S=$(uname -s)
UNAME_M=$(uname -m)

# Find a more useful OS name
case "$UNAME_S" in
  Linux*)     OS=linux;;
  Darwin*)    OS=macos;;
  CYGWIN*)    OS=cygwin;;
  MINGW*)     OS=mingw;;
  *)          OS="unknown:${UNAME_S}"
esac

# Expose compiler flags
. ${NXDK_DIR}/usr/share/nxdk/flags.inl

# Color codes
COLOR_YELLOW='\033[1;33m'
COLOR_RED='\033[0;31m'
COLOR_NONE='\033[0m'

# Helper to print a warning message
warning()
{
  echo -e "$(basename $0): ${COLOR_YELLOW}warning:${COLOR_NONE} $@"
}

# Helper to print an error message
error()
{
  echo -e "$(basename $0): ${COLOR_RED}error:${COLOR_NONE} $@\n"
}
