#include <cstdlib>
#include "src/core.hpp"

int main(int const argc, char const* argv[])
{
    core::Simulation simulation;

    simulation.initializeCore(argv[1]);
    simulation.launchSimulation();
    return EXIT_SUCCESS;
}
