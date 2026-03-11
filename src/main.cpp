#include <cstdlib>
#include "src/core.hpp"

int checkArg(int const argc, char const* argv[]) {
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    if (argv == nullptr || argv[1] == nullptr) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int main(int const argc, char const* argv[])
{
    core::Simulation simulation;

    if (checkArg(argc, argv) == EXIT_FAILURE)
        return EXIT_FAILURE;
    simulation.initializeCore(argv[1]);
    simulation.launchSimulation();
    return EXIT_SUCCESS;
}
