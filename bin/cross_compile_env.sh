# NO bin bash here, use source to get this!  
# change these to match your MinGW installation:
# platform name, for configure scripts
HOST="i586-mingw32msvc" 
# prefix for MinGW executables (e.g. if MinGW gcc is named i686-w64-mingw32-gcc, use "i686-w64-mingw32-")
MINGW_BIN_PREFIX="i586-mingw32msvc-"
# where to install the libraries
# you'll probably want to set this to the location where all the existing MinGW bin/lib/include folders are
MINGW_INSTALL_DIR="/usr/i586-mingw32msvc" 
export AR=${MINGW_BIN_PREFIX}ar
export CC=${MINGW_BIN_PREFIX}gcc
export RANLIB=${MINGW_BIN_PREFIX}ranlib
export PREFIX=${MINGW_INSTALL_DIR}
MAKE="make -j 2"

