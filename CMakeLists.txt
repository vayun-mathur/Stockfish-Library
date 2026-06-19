cmake_minimum_required(VERSION 3.22.1)
project(stockfish_lib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(STOCKFISH_SOURCES
    src/attacks.cpp
    src/benchmark.cpp
    src/bitboard.cpp
    src/engine.cpp
    src/evaluate.cpp
    src/main.cpp
    src/memory.cpp
    src/misc.cpp
    src/movegen.cpp
    src/movepick.cpp
    src/nnue/features/full_threats.cpp
    src/nnue/features/half_ka_v2_hm.cpp
    src/nnue/network.cpp
    src/nnue/nnue_accumulator.cpp
    src/nnue/nnue_misc.cpp
    src/position.cpp
    src/score.cpp
    src/search.cpp
    src/syzygy/tbprobe.cpp
    src/thread.cpp
    src/timeman.cpp
    src/tt.cpp
    src/tune.cpp
    src/uci.cpp
    src/ucioption.cpp
    src/libstockfish.cpp
    src/bridge.cpp
)

add_library(stockfish SHARED ${STOCKFISH_SOURCES})

target_include_directories(stockfish PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nnue
)

target_compile_definitions(stockfish PRIVATE
    STOCKFISH_LIBRARY
    NNUE_EMBEDDING_OFF
    __ANDROID__
    ANDROID
)

target_compile_options(stockfish PRIVATE -O3 -flto -DNDEBUG)
target_link_options(stockfish PRIVATE -flto)
