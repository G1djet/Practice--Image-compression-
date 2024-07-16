cmake_minimum_required(VERSION 3.10)
project(ImageCompressor)

# Поиск SFML библиотек и каталогов включения вручную
find_path(SFML_INCLUDE_DIR NAMES SFML/Graphics.hpp PATHS "D:/projects/practice_last/SFML/include")
find_library(SFML_GRAPHICS_LIBRARY NAMES sfml-graphics PATHS "D:/projects/practice_last/SFML/lib")
find_library(SFML_SYSTEM_LIBRARY NAMES sfml-system PATHS "D:/projects/practice_last/SFML/lib")
find_library(SFML_WINDOW_LIBRARY NAMES sfml-window PATHS "D:/projects/practice_last/SFML/lib")

# Включение каталога включения SFML
include_directories(${SFML_INCLUDE_DIR})

# Связывание библиотек SFML с целевым исполняемым файлом
add_executable(ImageCompressor main.cpp)
target_link_libraries(ImageCompressor ${SFML_GRAPHICS_LIBRARY} ${SFML_SYSTEM_LIBRARY} ${SFML_WINDOW_LIBRARY})