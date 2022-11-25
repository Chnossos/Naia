project(GameServer)

add_executable(${PROJECT_NAME} app.rc)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(${PROJECT_NAME} PRIVATE Shared)

set(SOURCES
        Application.cpp
        Character.cpp
        Chat.cpp
        Connection.cpp
        GameConstants.cpp
        HandleAccountTasks.cpp
        HandleBasicTasks.cpp
        HandleCommands.cpp
        HandleGameTasks.cpp
        HandleServerRequest.cpp
        Level.cpp
        main.cpp
        Map.cpp
        Session.cpp
        StorageProvider.cpp
        World.cpp
)
set(HEADERS
        Application.hpp
        Cell.hpp
        Character.hpp
        Chat.hpp
        ConfigDefault.hpp
        Connection.hpp
        GameConstants.hpp
        HandleAccountTasks.hpp
        HandleBasicTasks.hpp
        HandleCommands.hpp
        HandleGameTasks.hpp
        HandleServerRequest.hpp
        Level.hpp
        main.hpp
        Map.hpp
        PacketBuilder.hpp
        Session.hpp
        StorageProvider.hpp
        Unit.hpp
        World.hpp
)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES} ${HEADERS})
target_precompile_headers(${PROJECT_NAME} PRIVATE Naia.hpp)
