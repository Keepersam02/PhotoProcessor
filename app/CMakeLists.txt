# Stub bpp_lib so linker is happy while src is disabled
add_library(bpp_lib INTERFACE)

add_executable(bpp_app
  main.cpp
)

target_compile_options(bpp_app PRIVATE
  $<$<CONFIG:Debug>:-Wall -Wextra -Werror -Og>
)
target_compile_options(bpp_app PRIVATE
  $<$<CONFIG:Release>:-O3>
)

include(FetchContent)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG        v5.0.0
)
FetchContent_GetProperties(ftxui)

# Force static builds so all symbols bundle together
set(FTXUI_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(ftxui)

target_link_libraries(bpp_app PRIVATE
    bpp_lib
    -Wl,--start-group
    ftxui::screen
    ftxui::dom
    ftxui::component
    -Wl,--end-group

)
