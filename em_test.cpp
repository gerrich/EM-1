
#include "defs.h"
#include "em/vector.h"
#include "em/sort.h"

#include <ctime>

int main() {

  char fname[] = "out.txt";

  em::TFile f(fname, O_RDWR | O_CREAT | O_TRUNC, 0); 
 
  size_t cnt = 1024 * 1024 * 8; 
  em::window<int> w(f, 0, sizeof(int)*cnt);
  em::window<int>::iterator it = w.begin();
  srand(time(NULL));
  for(int i = 0; i < cnt; ++i) {
    *it = rand();
    ++it;
  }
  w.flush(f, 0);

  em::sort<int>(f, std::greater<int>());

  {
    const em::door<int> in_door(f, 0, f.size());
    if (!check_sorted(in_door.begin(), in_door.end(), std::greater<int>())) {
      cerr << "Range not sorted" << endl;
    }
  }

  return 0;
}
