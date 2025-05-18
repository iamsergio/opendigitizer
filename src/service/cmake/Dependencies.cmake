include(FetchContent)
include(DependenciesSHAs)

FetchContent_Declare(
        opencmw-cpp
        GIT_REPOSITORY https://github.com/fair-acc/opencmw-cpp.git
        GIT_TAG ${GIT_SHA_OPENCMW_CPP}
        SYSTEM)

FetchContent_Declare(
        gnuradio4
        SYSTEM
        SOURCE_DIR /data/sources/fair/gnuradio4/
)

FetchContent_Declare(
        gr-digitizers
        GIT_REPOSITORY https://github.com/fair-acc/gr-digitizers.git
        GIT_TAG ${GIT_SHA_GR_DIGITIZERS}
        SYSTEM
)

FetchContent_MakeAvailable(opencmw-cpp gr-digitizers gnuradio4)
