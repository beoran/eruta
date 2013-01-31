find_package(PkgConfig)

# Find main allegro and assume the rest is there too
pkg_check_modules(Allegro51_PKGCONF allegro-5.1)

# MESSAGE("lib: ${Allegro51_PKGCONF_LIBRARY_DIRS}")
# MESSAGE("include: ${Allegro51_PKGCONF_INCLUDE_DIRS}")

# Include dir
find_path(Allegro51_INCLUDE_DIR
  NAMES allegro5/allegro5.h
  PATHS ${Allegro51_PKGCONF_INCLUDE_DIRS}
)

# message("include dir: ${Allegro51_INCLUDE_DIR}")

# Names of all libraries in Allegro, without versions
set(ALLEGRO51_ALL_LIBRARIES
  allegro allegro_image allegro_font
  allegro_primitives allegro_ttf allegro_audio
  allegro_dialog allegro_memfile allegro_acodec
  allegro_color allegro_main allegro_physfs
)

# set(ALLEGRO51_LIBRARIES "")

# Find all libraries to link 
foreach(ALLEGRO51_ONE_LIBRARY ${ALLEGRO51_ALL_LIBRARIES})
  # message("${ALLEGRO51_ONE_LIBRARY}")
  find_library("${ALLEGRO51_ONE_LIBRARY}_AID" "${ALLEGRO51_ONE_LIBRARY}"
  ${Allegro51_PKGCONF_LIBRARY_DIRS}
  )  
  set(Allegro51_LIBRARIES
   "${Allegro51_LIBRARIES}" "${${ALLEGRO51_ONE_LIBRARY}_AID}")
  # MESSAGE("${Allegro51_LIBRARIES}")
endforeach(ALLEGRO51_ONE_LIBRARY)
# Remove first empty erroneous "library"
list(REMOVE_AT Allegro51_LIBRARIES 0)
# MESSAGE("${Allegro51_LIBRARIES}")
# message("${ALLEGRO51_LIBRARIES}")
# allegro_image allegro_font allegro_primitives
#   allegro_ttf allegro_audio allegro_dialog allegro_acodec
#   allegro_color allegro_main allegro_physfs



# Set the include dir variables and the libraries and let libfind_process do the rest.
set(ALLEGRO_INCLUDE_DIR "${Allegro51_INCLUDE_DIR}")
set(ALLEGRO_LIBRARIES "${Allegro51_LIBRARIES}")
# libfind_process(Allegro51)

# MESSAGE("all libs: ${ALLEGRO_LIBRARIES}")
# MESSAGE("all include: ${ALLEGRO_INCLUDE_DIR}")

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Allegro51 DEFAULT_MSG ALLEGRO_LIBRARIES ALLEGRO_INCLUDE_DIR)

MARK_AS_ADVANCED(ALLEGRO_INCLUDE_DIR ALLEGRO_LIBRARIES)



# MESSAGE("${ALLEGRO_LIBRARIES}")
