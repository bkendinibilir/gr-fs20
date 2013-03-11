# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
add_library(gnuradio-fs20 SHARED
    fs20_bit_decoder_bb_impl.cc
    fs20_frame_decoder_b_impl.cc )
target_link_libraries(gnuradio-fs20 ${Boost_LIBRARIES} ${GRUEL_LIBRARIES} ${GNURADIO_CORE_LIBRARIES})
set_target_properties(gnuradio-fs20 PROPERTIES DEFINE_SYMBOL "gnuradio_fs20_EXPORTS")

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-fs20
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_fs20_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/test_fs20.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/qa_fs20.cc
)

add_executable(test-fs20 ${test_fs20_sources})

target_link_libraries(
  test-fs20
  ${GNURADIO_CORE_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-fs20
)

GR_ADD_TEST(test_fs20 test-fs20)
