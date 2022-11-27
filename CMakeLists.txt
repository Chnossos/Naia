cmake_minimum_required(VERSION 3.16)
project(Naia VERSION 0.53.45.40)

if(MSVC)
    add_compile_options(
        /wd4267 # numeric conversions, possible loss of data
    )
endif()

add_subdirectory(Shared)
add_subdirectory(LoginServer)
add_subdirectory(GameServer)

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets Xml Sql)
if(Qt5_FOUND)
    add_subdirectory(GSRegister)
else()
    message(Qt5 not found, GameServerRegister will not be built)
endif()
