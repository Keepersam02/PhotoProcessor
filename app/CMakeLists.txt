add_executable(bpp_app main.cpp)

target_compile_options(bpp_app PRIVATE
  $<$<CONFIG:Debug>:
  -Wall;
  -Wextra;
  -Wpedantic;
  -g;
  -O1
  -Wshadow;
  -Wunused;
  >
)
target_link_libraries(bpp_app PRIVATE bpp_lib)
