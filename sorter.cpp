#include "defs.h"
#include "em/door.h"
#include "em/sort.h"

#include <algorithm>
#include <fstream>
#include <iterator>
//#include <vector>

// Генератор счетов.

void print_usage(const char* program) {
    std::cout
        << "Usage: " << program << " <parsed-file> <sorted-file> [memory]"
        << std::endl;

    std::cout << "parsed-file\tinput file" << std::endl;
    std::cout << "sorted-file\toutput file" << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    em::TFile parsed_log(argv[1], O_RDONLY);
    const em::door<LogEntry> in_door(parsed_log, 0, parsed_log.size());

    em::TFile sorted_log(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0);
    {
      em::door<LogEntry> out_door(sorted_log, 0, 0);
      // Копируем данные, чтобы не испортить исходник 
      std::copy(in_door.begin(), in_door.end(), out_door.begin());
    }
    
    // Сортируем файлик
    em::sort<LogEntry>(sorted_log, SortByCaller());

    std::cout << "#========== IO stats ==========" << std::endl;
    em::TIOStat::print_stat(std::cout);
    std::cout << std::endl;
    std::cout << std::endl;

    if (0) {
      const em::door<LogEntry> in_door(sorted_log, 0, sorted_log.size());
      if (!check_sorted(in_door.begin(), in_door.end(), SortByCaller())) {
        cerr << "Range not sorted" << endl;
      }
    }

    return 0;
}

