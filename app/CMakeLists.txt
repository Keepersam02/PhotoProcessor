add_executable(bpp_app
  main.cpp
)

target_compile_options(bpp_app PRIVATE
  $<$<CONFIG:Debug>:-Wall -Wextra -Werror -Og>
)
target_compile_options(bpp_app PRIVATE
  $<$<CONFIG:Release>:-O3>
)

target_link_libraries(bpp_app PRIVATE
  bpp_lib
)
