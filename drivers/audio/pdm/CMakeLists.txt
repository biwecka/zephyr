# SPDX-License-Identifier: Apache-2.0

# if(CONFIG_PDM)
#   # Add hello_world_driver.h to the set of global include paths.
#   zephyr_include_directories(.)

#   zephyr_library()
#   zephyr_library_sources(
#     pdm.c
#   )
# endif()

zephyr_library()

zephyr_library_sources_ifdef(CONFIG_PDM pdm.c)
