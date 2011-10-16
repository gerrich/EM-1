#include "defs.h"
#include "em/door.h"

#include <algorithm>
#include <fstream>
#include <iterator>
//#include <vector>

// Генератор счетов.

void print_usage(const char* program)
{
    std::cout
        << "Usage: " << program << " <log-file> <bill-file> [memory]"
        << std::endl;

    std::cout << " log-file\tinput file" << std::endl;
    std::cout << "bill-file\toutput file" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        print_usage(argv[0]);
        return 1;
    }

    // Читаем лог событий.
    std::fstream in(argv[1], std::ios::in);
    em::TFile f(argv[2]);
    em::door<LogEntry> out_door(f, 0, 0);
    std::copy(
        std::istream_iterator<LogEntry>(in),
        std::istream_iterator<LogEntry>(),
        out_door.begin());

    return 0;
}

