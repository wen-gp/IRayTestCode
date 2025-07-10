set(PROJECT_NAME TbdpApplication)
message(STATUS "加载${PROJECT_NAME}中")

file(GLOB SOURCES "*.cpp")

if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
else()
    add_executable(${PROJECT_NAME} ${SOURCES})
endif()

set(IRAY_INCLUDE "${CMAKE_SOURCE_DIR}/SDK/IRay/libir_SDK_release/include")
set(IRAY_LIB "${CMAKE_SOURCE_DIR}/SDK/IRay/libir_SDK_release/linux/x64")

set(DEPENDENCIES  )

target_include_directories(${PROJECT_NAME} PUBLIC ${IRAY_INCLUDE} ${DEPENDENCIES})

target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC 
    "${IRAY_LIB}/libircam.a"
    "${IRAY_LIB}/libircmd.a"
    "${IRAY_LIB}/libirdfu.a"
    "${IRAY_LIB}/libiri2c.a"
    "${IRAY_LIB}/libirinfoparse.a"
    "${IRAY_LIB}/libirparse.a"
    "${IRAY_LIB}/libirspi.a"
    "${IRAY_LIB}/libirtemp.a"
    "${IRAY_LIB}/libirupgrade.a"
    "${IRAY_LIB}/libirupgrade.a"
    "${IRAY_LIB}/libiruvc.a"
    "${IRAY_LIB}/libirv4l2.a"
    "${IRAY_LIB}/libusb-1.0.a"
    ${DEPENDENCIES}
)

message(STATUS "${PROJECT_NAME}加载完毕")