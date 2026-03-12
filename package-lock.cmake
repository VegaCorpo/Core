#entt
CPMDeclarePackage(EnTT
    GIT_TAG v3.16.0
    GITHUB_REPOSITORY skypjack/entt
    SYSTEM YES
    EXCLUDE_FROM_ALL YES
)

# Common
CPMDeclarePackage(Common
        GIT_TAG load_texture_from_pixels
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
