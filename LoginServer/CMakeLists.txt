project(LoginServer)

add_executable(${PROJECT_NAME} app.rc)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(${PROJECT_NAME} PRIVATE Shared)

set(SOURCES
        Application.cpp
        GameServer.cpp
        LoginServer.cpp
        main.cpp
        Session.cpp
        TaskHandler.cpp
)
set(HEADERS
        Application.hpp
        ConfigDefault.hpp
        GameServer.hpp
        LoginServer.hpp
        Session.hpp
        TaskHandler.hpp
)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES} ${HEADERS})
target_precompile_headers(${PROJECT_NAME} PRIVATE Naia.hpp)
