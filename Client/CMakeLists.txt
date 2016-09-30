project(Client)
set(PROJECT_SOURCES main.cpp)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME} nanomsg)
