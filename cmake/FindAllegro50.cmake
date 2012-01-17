find_package(PkgConfig)

# Find main allegro and assume the rest is there too
pkg_check_modules(Allegro50_PKGCONF allegro-5.0)

# MESSAGE("lib: ${Allegro50_PKGCONF_LIBRARY_DIRS}")
# MESSAGE("include: ${Allegro50_PKGCONF_INCLUDE_DIRS}")

# Include dir
find_path(Allegro50_INCLUDE_DIR
  NAMES allegro5/allegro5.h
  PATHS ${Allegro50_PKGCONF_INCLUDE_DIRS}
)

# message("include dir: ${Allegro50_INCLUDE_DIR}")

# Names of all libraries in Allegro, without versions
set(ALLEGRO50_ALL_LIBRARIES
  allegro allegro_image allegro_font
  allegro_primitives allegro_ttf allegro_audio
  allegro_dialog allegro_memfile allegro_acodec
  allegro_color allegro_main allegro_physfs
)

# set(ALLEGRO50_LIBRARIES "")

# Find all libraries to link 
foreach(ALLEGRO50_ONE_LIBRARY ${ALLEGRO50_ALL_LIBRARIES})
  # message("${ALLEGRO50_ONE_LIBRARY}")
  find_library("${ALLEGRO50_ONE_LIBRARY}_AID" "${ALLEGRO50_ONE_LIBRARY}"
  ${Allegro50_PKGCONF_LIBRARY_DIRS}
  )  
  set(Allegro50_LIBRARIES
   "${Allegro50_LIBRARIES}" "${${ALLEGRO50_ONE_LIBRARY}_AID}")
  # MESSAGE("${Allegro50_LIBRARIES}")
endforeach(ALLEGRO50_ONE_LIBRARY)
# Remove first empty erroneous "library"
list(REMOVE_AT Allegro50_LIBRARIES 0)
# MESSAGE("${Allegro50_LIBRARIES}")
# message("${ALLEGRO50_LIBRARIES}")
# allegro_image allegro_font allegro_primitives
#   allegro_ttf allegro_audio allegro_dialog allegro_acodec
#   allegro_color allegro_main allegro_physfs



# Set the include dir variables and the libraries and let libfind_process do the rest.
set(ALLEGRO_INCLUDE_DIR "${Allegro50_INCLUDE_DIR}")
set(ALLEGRO_LIBRARIES "${Allegro50_LIBRARIES}")
# libfind_process(Allegro50)

# MESSAGE("all libs: ${ALLEGRO_LIBRARIES}")
# MESSAGE("all include: ${ALLEGRO_INCLUDE_DIR}")

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Allegro50 DEFAULT_MSG ALLEGRO_LIBRARIES ALLEGRO_INCLUDE_DIR)

MARK_AS_ADVANCED(ALLEGRO_INCLUDE_DIR ALLEGRO_LIBRARIES)



# MESSAGE("${ALLEGRO_LIBRARIES}")
