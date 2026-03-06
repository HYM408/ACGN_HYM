cmake_minimum_required(VERSION 3.16)

project(ACGN_HYM VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Sql
        Network
        WebEngineWidgets
)

set(LIBXML2_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(LIBXML2_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/libxml2/libxml2.lib")

set(VLC_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(VLC_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/vlc/libvlc.lib"
                  "${CMAKE_SOURCE_DIR}/lib/vlc/libvlccore.lib")

# 主程序
add_executable(appACGN_HYM
        windows/main.cpp
        windows/api/bangumi_api.h
        windows/api/bangumi_api.cpp
        windows/api/bangumi_oauth.h
        windows/api/bangumi_oauth.cpp
        windows/api/pikpak_api.h
        windows/api/pikpak_api.cpp
        windows/crawler/crawler.h
        windows/crawler/crawler.cpp
        windows/downloader/chunk_download.h
        windows/downloader/chunk_download.cpp
        windows/downloader/download_page.h
        windows/downloader/download_page.cpp
        windows/player/player.h
        windows/player/player.cpp
        windows/player/player_core.h
        windows/player/player_core.cpp
        windows/player/player_page.h
        windows/player/player_page.cpp
        windows/sql/data_structs.h
        windows/sql/sql.h
        windows/sql/sql.cpp
        windows/utils/cache_image_util.h
        windows/utils/cache_image_util.cpp
        windows/utils/image_util.h
        windows/utils/image_util.cpp
        windows/utils/menu_util.h
        windows/utils/menu_util.cpp
        windows/utils/network_util.h
        windows/utils/network_util.cpp
        windows/utils/progress_util.h
        windows/utils/progress_util.cpp
        windows/utils/xml_util.h
        windows/utils/xml_util.cpp
        windows/config.h
        windows/config.cpp
        windows/detail_page.h
        windows/detail_page.cpp
        windows/episode_page.h
        windows/episode_page.cpp
        windows/main_page.h
        windows/main_page.cpp
        windows/rss.h
        windows/rss.cpp
        windows/search_page.h
        windows/search_page.cpp
        windows/settings_page.h
        windows/settings_page.cpp
        windows/window.h
        windows/window.cpp
)

target_link_libraries(appACGN_HYM
        PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Sql
        Qt6::Network
        ${LIBXML2_LIBRARIES}
        ${VLC_LIBRARIES}
)

target_include_directories(appACGN_HYM PRIVATE
        ui
        ${LIBXML2_INCLUDE_DIR}
        ${VLC_INCLUDE_DIR}
)

# WebEngine进程
add_executable(videoStream
        windows/crawler/video_stream.cpp
)

target_link_libraries(videoStream
        PRIVATE
        Qt6::WebEngineWidgets
)

add_dependencies(appACGN_HYM videoStream)


if(WIN32)
    add_custom_command(TARGET appACGN_HYM POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_SOURCE_DIR}/lib/libxml2/libxml2.dll"
                "${CMAKE_SOURCE_DIR}/lib/libxml2/iconv-2.dll"
                "${CMAKE_SOURCE_DIR}/lib/libxml2/zlib1.dll"
                "${CMAKE_SOURCE_DIR}/lib/vlc/libvlc.dll"
                "${CMAKE_SOURCE_DIR}/lib/vlc/libvlccore.dll"
                "${CMAKE_SOURCE_DIR}/windows/crawler/XPath.json"
            $<TARGET_FILE_DIR:appACGN_HYM>

            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/icons"
            $<TARGET_FILE_DIR:appACGN_HYM>/icons

            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/plugins"
            $<TARGET_FILE_DIR:appACGN_HYM>/plugins
    )
endif()