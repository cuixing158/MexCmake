cmake_minimum_required(VERSION 3.18)
set(CMAKE_C_COMPILER "D:/mingw64_v8/bin/x86_64-w64-mingw32-gcc.exe")
set(CMAKE_CXX_COMPILER "D:/mingw64_v8/bin/x86_64-w64-mingw32-g++.exe")
project(gccMex DESCRIPTION "基于CMake交叉编译C/C++得到适用与Matlab的Mex加速库文件"
    VERSION 1.0
    HOMEPAGE_URL "https://github.com/cuixing158")
message(STATUS "Use CMAKE version:${CMAKE_VERSION}")
find_package(Matlab REQUIRED) # https://cmake.org/cmake/help/latest/module/FindMatlab.html

if(MATLAB_FOUND)
    message(STATUS "MATLAB Found, MATLAB MEX will be compiled.")
    set(MATLAB_INSTALL_ROOT ${Matlab_ROOT_DIR})
    message(STATUS ${MATLAB_INSTALL_ROOT})
else()
    message(FATAL_ERROR "MATLAB not found...nothing will be built.")
endif()

if(WIN32)
    message(STATUS "Now is windows")

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        message(STATUS "using MSVC complier.")
        file(GLOB mexLibs ${MATLAB_INSTALL_ROOT}/extern/lib/win64/microsoft/*.lib)
    else("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        message(STATUS "using gcc complier.")
        file(GLOB mexLibs ${MATLAB_INSTALL_ROOT}/extern/lib/win64/mingw64/*.lib)
    endif()
elseif(UNIX)
    message(STATUS "Now is UNIX-like OS's.")
    file(GLOB mexLibs ${MATLAB_INSTALL_ROOT}/extern/lib/linux64/gcc/*.a)
elseif(APPLE)
    message(STATUS "Now is Apple systens.")
    file(GLOB mexLibs ${MATLAB_INSTALL_ROOT}/extern/lib/maci64/clang/*.a)
else()
    message(FATAL_ERROR "Now this platform is not support.")
endif()

if(NOT mexLibs)
    message(FATAL_ERROR "没找到包含的库文件.")
endif(NOT mexLibs)

foreach(item IN LISTS mexLibs)
    message(STATUS "库文件为:${item}")
endforeach(item IN LISTS mexLibs)

message(STATUS "列出依赖的lib完毕")

aux_source_directory(. srcLists)
message(STATUS ${srcLists})
add_library(myGccMex MODULE ${srcLists})
target_include_directories(myGccMex PUBLIC ${MATLAB_INSTALL_ROOT}/extern/include)
target_link_libraries(myGccMex ${mexLibs})
target_compile_features(myGccMex INTERFACE cxx_std_11)

if(WIN32)
    set_target_properties(myGccMex PROPERTIES OUTPUT_NAME "myGccMex.mexw64"
        PREFIX "" # 前缀为空
        SUFFIX "") # 后缀为空
elseif(UNIX)
    set_target_properties(myGccMex PROPERTIES OUTPUT_NAME "myGccMex.mexa64"
        PREFIX ""
        SUFFIX "")
elseif(APPLE)
    set_target_properties(myGccMex PROPERTIES OUTPUT_NAME "myGccMex.mexmaci64"
        PREFIX ""
        SUFFIX "")
else()
    message(FATAL_ERROR "Now this platform is not support.")
endif()

# 设置安装路径设置
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)
install(TARGETS myGccMex
    EXPORT ${PROJECT_NAME}_Targets # 生成.cmake文件
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
message(STATUS "执行make install, 安装路径为：${CMAKE_BINARY_DIR}/install")

# 关于mex调试另请参考：https://blogs.mathworks.com/developer/2018/06/19/mex-debugging-vscode/