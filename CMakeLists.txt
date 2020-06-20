cmake_minimum_required(VERSION 3.1.0)
project(V-2DFramework VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set(THREADS_PREFER_PTHREAD_FLAG ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  file(GLOB SDL2_SHARED_BINARIES
    "deps/SFML/bin/*.dll"
    "deps/Box2D/bin/*.dll"
  )
  file(COPY ${SDL2_SHARED_BINARIES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
  file(COPY "res" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
else()
  find_package(SFML REQUIRED network audio graphics window system)
endif()

find_package(Threads REQUIRED)

add_definitions(
  -DSFML_STATIC
  -DSFGUI_STATIC
  -DLOG_EXCEPTION
)


file(GLOB_RECURSE ALL_SOURCES "${CMAKE_SOURCE_DIR}/*.cpp")


set(ClientSRC "${ALL_SOURCES}")
list(REMOVE_ITEM ClientSRC "${CMAKE_SOURCE_DIR}/MainServer.cpp")

set(ServerSRC "${ALL_SOURCES}")
list(REMOVE_ITEM ServerSRC "${CMAKE_SOURCE_DIR}/MainClient.cpp")

add_executable(Client "${ClientSRC}")
add_executable(Server "${ServerSRC}")


############
## CLIENT ##
############

target_include_directories(Client PRIVATE
  src
  src/apps
  src/core
  src/etc
  src/input
  src/interfaces
  src/lightning
  src/net
  src/netmodules
  src/physics
  src/project
  src/project/pathfinders
  src/res
  src/screens
  src/throwmacros
)

set_target_properties(Client PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
  )

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_include_directories(Client PRIVATE
    deps/SFML/include
    deps/SFGUI/include
    deps/Box2D/include
  )
  target_link_directories(Client PRIVATE
    deps/SFML/lib
    deps/SFGUI/lib
    deps/Box2D/lib
  )
  target_link_libraries(Client PRIVATE
    Box2D
    SFGUI
    sfml-graphics-s
    sfml-window-s
    sfml-audio-s
    sfml-network-s
    sfml-system-s
    opengl32
    freetype
    winmm
    gdi32
    openal32
    flac
    vorbisenc
    vorbisfile
    vorbis
    ogg
    ws2_32
  )
else() 
  target_include_directories(Client PRIVATE
    ${SFML_INCLUDE_DIR}
  )
  target_link_libraries(Client PRIVATE
    ${SFML_LIBRARIES}
    Threads::Threads
  )
endif()


############
## SERVER ##
############

target_include_directories(Server PRIVATE
  src
  src/apps
  src/core
  src/etc
  src/input
  src/interfaces
  src/lightning
  src/physics
  src/project
  src/net
  src/netmodules
  src/res
  src/screens
  src/primitives
  src/throwmacros
)

set_target_properties(Server PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
  )

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_include_directories(Server PRIVATE
    deps/SFML/include
    deps/SFGUI/include
    deps/Box2D/include
  )
  target_link_directories(Server PRIVATE
    deps/SFML/lib
    deps/SFGUI/lib
    deps/Box2D/lib
  )
  target_link_libraries(Server PRIVATE
    Threads::Threads
    sfml-graphics-s
    sfml-window-s
    sfml-audio-s
    sfml-network-s
    sfml-system-s
    opengl32
    freetype
    winmm
    gdi32
    openal32
    flac
    vorbisenc
    vorbisfile
    vorbis
    ogg
    ws2_32
    SFGUI
  )
else() 
  target_include_directories(Server PRIVATE
    ${SFML_INCLUDE_DIR}
  )
  target_link_libraries(Server PRIVATE
    ${SFML_LIBRARIES}
    Threads::Threads
  )
endif()