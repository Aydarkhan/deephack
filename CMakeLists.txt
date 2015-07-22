cmake_minimum_required (VERSION 2.6)
project(dqn)

option(CPU_ONLY "Use CPU only for Caffe" ON)
option(USE_CUDNN "Use cuDNN for Caffe" OFF)
option(USE_SDL "Use SDL for ALE" ON)

include_directories(~/Documents/projects/deephack/caffe-dqn/include)
include_directories(~/Documents/projects/deephack/Arcade-Learning-Environment/src)
link_directories(~/Documents/projects/deephack/caffe-dqn/build/lib)
link_directories(~/Documents/projects/deephack/Arcade-Learning-Environment/)
link_directories(/usr/local/lib/)


#include_directories(/opt/OpenBLAS/include)

if(USE_CUDNN)
  link_directories(~/cudnn-6.5-linux-R1)
endif()

add_executable(dqn dqn_main.cpp dqn.cpp)
add_executable(dqn_gopher dqn.cpp dqn_main_gopher_submit.cpp)
add_executable(dqn_seaquest dqn.cpp dqn_main_seaquest_submit.cpp)
add_executable(dqn_tutankham dqn.cpp dqn_main_tutankham_submit.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -march=native")

target_link_libraries(dqn caffe)
target_link_libraries(dqn glog)
target_link_libraries(dqn gflags)
target_link_libraries(dqn ale)

target_link_libraries(dqn_gopher caffe)
target_link_libraries(dqn_gopher glog)
target_link_libraries(dqn_gopher gflags)
target_link_libraries(dqn_gopher ale)

target_link_libraries(dqn_seaquest caffe)
target_link_libraries(dqn_seaquest glog)
target_link_libraries(dqn_seaquest gflags)
target_link_libraries(dqn_seaquest ale)

target_link_libraries(dqn_tutankham caffe)
target_link_libraries(dqn_tutankham glog)
target_link_libraries(dqn_tutankham gflags)
target_link_libraries(dqn_tutankham ale)

if(APPLE)
  include_directories(/System/Library/Frameworks/vecLib.framework/Versions/Current/Headers)
endif()

if(USE_SDL)
  add_definitions(-D__USE_SDL)
  if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Cocoa")
    target_link_libraries(dqn sdl)
    target_link_libraries(dqn sdl_gfx)
    target_link_libraries(dqn sdl_image)
    target_link_libraries(dqn sdlmain)

    target_link_libraries(dqn_gopher sdl)
    target_link_libraries(dqn_gopher sdl_gfx)
    target_link_libraries(dqn_gopher sdl_image)
    target_link_libraries(dqn_gopher sdlmain)

    target_link_libraries(dqn_seaquest sdl)
    target_link_libraries(dqn_seaquest sdl_gfx)
    target_link_libraries(dqn_seaquest sdl_image)
    target_link_libraries(dqn_seaquest sdlmain)

    target_link_libraries(dqn_tutankham sdl)
    target_link_libraries(dqn_tutankham sdl_gfx)
    target_link_libraries(dqn_tutankham sdl_image)
    target_link_libraries(dqn_tutankham sdlmain)
  else()
    target_link_libraries(dqn SDL)
    target_link_libraries(dqn SDL_gfx)
    target_link_libraries(dqn SDL_image)
    target_link_libraries(dqn SDLmain)

    target_link_libraries(dqn_gopher SDL)
    target_link_libraries(dqn_gopher SDL_gfx)
    target_link_libraries(dqn_gopher SDL_image)
    target_link_libraries(dqn_gopher SDLmain)

    target_link_libraries(dqn_seaquest SDL)
    target_link_libraries(dqn_seaquest SDL_gfx)
    target_link_libraries(dqn_seaquest SDL_image)
    target_link_libraries(dqn_seaquest SDLmain)

    target_link_libraries(dqn_tutankham SDL)
    target_link_libraries(dqn_tutankham SDL_gfx)
    target_link_libraries(dqn_tutankham SDL_image)
    target_link_libraries(dqn_tutankham SDLmain)
  endif()
endif()

if(NOT CPU_ONLY)
  include_directories(/usr/local/cuda-6.5/targets/x86_64-linux/include)
endif()

if(CPU_ONLY)
  add_definitions(-DCPU_ONLY)
endif()

if(USE_CUDNN)
  target_link_libraries(dqn cudnn)
  target_link_libraries(dqn_gopher cudnn)
  target_link_libraries(dqn_seaquest cudnn)
  target_link_libraries(dqn_tutankham cudnn)
endif()

