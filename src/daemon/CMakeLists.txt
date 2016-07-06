CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(mtp-initiator C)

MESSAGE("VERSION : ${FULLVER}, MAJOR : ${MAJORVER}")
MESSAGE("CMAKE_INSTALL_LIBDIR: ${CMAKE_INSTALL_LIBDIR}")
MESSAGE("INCLUDE_INSTALL_DIR : ${INCLUDE_INSTALL_DIR}")
MESSAGE("LIB_INSTALL_DIR : ${LIB_INSTALL_DIR}")
MESSAGE("SHARE_INSTALL_PREFIX : ${SHARE_INSTALL_PREFIX}")
MESSAGE("SYSCONF_INSTALL_DIR : ${SYSCONF_INSTALL_DIR}")
MESSAGE("BUILD_SHARED_LIBS : ${BUILD_SHARED_LIBS}")

FIND_PROGRAM(GDBUS_CODEGEN NAMES gdbus-codegen)
EXEC_PROGRAM(${GDBUS_CODEGEN} ARGS
                " \\
                --generate-c-code ${CMAKE_CURRENT_SOURCE_DIR}/mtp_gdbuslib \\
                --c-namespace mtp_gdbuslib \\
                --interface-prefix org.tizen.mtp. \\
                ${CMAKE_CURRENT_SOURCE_DIR}/mtp_gdbuslib.xml \\
                ")

FILE(GLOB SRCS *)

include_directories(${CMAKE_SOURCE_DIR}/src/daemon)
include_directories(${CMAKE_SOURCE_DIR}/src/daemon/include)

INCLUDE(FindPkgConfig)
	pkg_check_modules(mtp_pkgs REQUIRED glib-2.0 gio-2.0 libmtp dlog gio-unix-2.0 sqlite3 capi-base-common libtzplatform-config)

FOREACH(flag ${mtp_pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} -fpic -Wall -Werror-implicit-function-declaration -fvisibility=hidden")
SET(ARM_CFLAGS "${ARM_CFLAGS} -mapcs -mabi=aapcs-linux -msoft-float -Uarm -fpic")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")

SET(CMAKE_EXE_LINKER_FLAGS " -Wl,--as-needed -pie -Wl,--hash-style=both")

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${mtp_pkgs_LDFLAGS} pthread)

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
