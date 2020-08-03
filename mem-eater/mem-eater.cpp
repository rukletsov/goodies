
#include <vector>
#include <unistd.h>

using namespace std;

int main() {

  const int GB = 10;
  const size_t BYTES_IN_GB = 1024*1024*1024;
  const int SLEEP_DURATION = 100;

  vector<vector<char>*> v;

  for (int i = 1; i < GB; ++i) {
    vector<char>* tmp = new vector<char>(BYTES_IN_GB, 1);
    v.push_back(tmp);
  }

  sleep(SLEEP_DURATION);

  return 0;
}
