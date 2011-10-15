
#include "defs.h"
#include "em/vector.h"
#include "em/sort.h"


int main() {

  char fname[] = "out.txt";

  em::TFile f(fname, sizeof(int)*1e5); 
  em::window<int> w(f, 0, sizeof(int)*1e6);
  em::window<int>::iterator it = w.begin();
  for(int i = 0; i < 1000000; ++i) {
    *it = i;
    ++it;
  }
  w.flush(f, 0);

  em::sort(f);

  return 0;
}
