# definitions in macros
ADD_DEFINITIONS(-DGNU_EFI_USE_MS_ABI -D_ARCH_=${ARCH} -D_BOARD_=${BOARD})
	
SET(CMAKE_C_FLAGS_DEBUG "")
SET(CMAKE_CXX_FLAGS_DEBUG "")
SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

INCLUDE(${BOARD_CMAKE_DIR}/components.cmake)

# Add subproject
ADD_SUBDIRECTORY(libarch)
ADD_SUBDIRECTORY(libbsp)
IF(${HAS_BOOTLOADER})
	ADD_SUBDIRECTORY(bootloader)
ENDIF()
ADD_SUBDIRECTORY(kernel)
