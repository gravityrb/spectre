# Distributed under the MIT License.
# See LICENSE.txt for details.

add_subdirectory(Python)
add_subdirectory(TimeDependence)

set(LIBRARY "Test_DomainCreators")

set(LIBRARY_SOURCES
  Test_AlignedLattice.cpp
  Test_BinaryCompactObject.cpp
  Test_BinaryCompactObjectHelpers.cpp
  Test_BlockGroups.cpp
  Test_Brick.cpp
  Test_Cylinder.cpp
  Test_CylindricalBinaryCompactObject.cpp
  Test_Disk.cpp
  Test_ExpandOverBlocks.cpp
  Test_FrustalCloak.cpp
  Test_Interval.cpp
  Test_Tags.cpp
  Test_Rectangle.cpp
  Test_RotatedBricks.cpp
  Test_RotatedIntervals.cpp
  Test_RotatedRectangles.cpp
  Test_ShapeMapOptions.cpp
  Test_Sphere.cpp
  Test_SphereTimeDependentMaps.cpp
  )

add_test_library(${LIBRARY} "${LIBRARY_SOURCES}")

target_link_libraries(
  ${LIBRARY}
  PRIVATE
  Domain
  DomainBoundaryConditions
  DomainBoundaryConditionsHelpers
  DomainCreators
  DomainHelpers
  H5
  SphericalHarmonics
  SphericalHarmonicsIO
  )
