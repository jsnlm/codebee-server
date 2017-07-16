#include <iomanip>
#include <iostream>
using namespace std;

int main( int argc, char* argv[] ) {
  cout << argc << " environment variable(s) were entered." << endl;
  for (int i = 1; i < argc; i++) {
    cout << "arg" << i << ": " << argv[i] << endl;
  }
  return 0;
}
