# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/ajayshanker/esp/esp-idf/components/bootloader/subproject"
  "/Users/ajayshanker/Developer/iot/esp32-lcd-restaurant/build/bootloader"
  "/Users/ajayshanker/Developer/iot/esp32-lcd-restaurant/build/bootloader-prefix"
  "/Users/ajayshanker/Developer/iot/esp32-lcd-restaurant/build/bootloader-prefix/tmp"
  "/Users/ajayshanker/Developer/iot/esp32-lcd-restaurant/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/ajayshanker/Developer/iot/esp32-lcd-restaurant/build/bootloader-prefix/src"
  "/Users/ajayshanker/Developer/iot/esp32-lcd-restaurant/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/ajayshanker/Developer/iot/esp32-lcd-restaurant/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/ajayshanker/Developer/iot/esp32-lcd-restaurant/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
