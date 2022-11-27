project(Shared)

find_package(Boost          REQUIRED COMPONENTS date_time thread)
find_package(libmysqlclient REQUIRED)

add_library(${PROJECT_NAME} OBJECT)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Boost::date_time
        Boost::thread
        libmysqlclient::libmysqlclient
)

set(SOURCES
        Config.cpp
        Database.cpp
        Kernel.cpp
        LanguageManager.cpp
        Log.cpp
        MersenneTwister.cpp
        md5.c
        ProgressUnit.cpp
        ThreadPool.cpp
        Utils.cpp
)
set(HEADERS
        _Common.hpp
        Config.hpp
        ConfigDefault.hpp
        Connector.hpp
        Constants.hpp
        Database.hpp
        Kernel.hpp
        LanguageManager.hpp
        Log.hpp
        MersenneTwister.hpp
        md5.h
        Packet.hpp
        PacketHeader.hpp
        ProgressUnit.hpp
        Singleton.hpp
        ThreadPool.hpp
        Timer.hpp
        Utils.hpp
)
set_source_files_properties(md5.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(MersenneTwister.cpp PROPERTIES COMPILE_OPTIONS $<$<BOOL:${MSVC}>:/wd4293>)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES} ${HEADERS})
