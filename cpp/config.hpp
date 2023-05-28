#include <algorithm>
#include <vector>

namespace config {
// TODO fix this name
using code_point = unsigned short int;

struct Config {
  std::vector<code_point> pattern;
};

} // namespace config
