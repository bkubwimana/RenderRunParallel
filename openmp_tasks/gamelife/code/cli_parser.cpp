
/*
Author: Benjamin Kubwimana
Class: ECE4122 or ECE6122 (section Q)
Last Date Modified: 10/3/2024
Description:
A header file that contains the implmenation for cli inputs used in the Game of Life simulation
*/

#include "cli_parser.h"
#include "clipp.h"

using namespace clipp;
using namespace std;

CommandLineParser::CommandLineParser()
    : mode(Mode::Threads), numThreads(8), size(5), width(800), height(600) {}

bool CommandLineParser::parseArguments(int argc, char **argv)
{
    auto cli = (option("-x", "--width") & integer("WIDTH").set(width).doc("Width of the window (default=" + std::to_string(width) + ")"),
                option("-y", "--height") & integer("HEIGHT").set(height).doc("Height of the window (default=" + std::to_string(height) + ")"),
                option("-c", "--size") & integer("SIZE").set(size).doc("Size in pixels of each cell (default=" + std::to_string(size) + ")"),
                option("-n", "--threads") & integer("numThreads").set(numThreads).doc("Number of threads to use (>2) (default=" + std::to_string(numThreads) + ")"),
                option("-t", "--mode") & one_of(
                                             required("SEQ").set(mode, Mode::Sequential),
                                             required("THRD").set(mode, Mode::Threads),
                                             required("OMP").set(mode, Mode::OpenMp))
                                             .doc("Type of parallelism to use (default: Sequential)"));

    if (!parse(argc, argv, cli))
    {
        auto fmt = doc_formatting{}
                       .first_column(2)
                       .doc_column(31)
                       .max_flags_per_param_in_usage(4);

        std::cout << "Usage:\n"
                  << usage_lines(cli, argv[0], fmt)
                  << "\n\nOptions:\n"
                  << documentation(cli, fmt)
                  << std::endl;
        return false;
    }

    return true;
}
