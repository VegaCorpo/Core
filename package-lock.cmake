# Common
CPMDeclarePackage(Common
        GIT_TAG features/camera_mouse_rotation
        GITHUB_REPOSITORY VegaCorpo/Common
        SYSTEM YES
        EXCLUDE_FROM_ALL YES
)

# boost
CPMDeclarePackage(Boost
        URL https://github.com/boostorg/boost/releases/download/boost-1.89.0/boost-1.89.0-cmake.tar.gz
        SYSTEM YES
        EXCLUDE_FROM_ALL YES
)
