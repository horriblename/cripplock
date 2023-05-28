#include "config.hpp"
#include "gtk/gtk.h"

struct RuntimeData {
  config::Config config;

  void push_to_sequence(config::code_point code) {
    if (code < 1 || code > 9) {
      // throw exception?
      return;
    }

    if (!occupied[code]) {
      this->accumulated_sequence.push_back(code);
      occupied[code] = true;
    }
  };

  void reset_sequence() {
    this->accumulated_sequence.clear();
    for (auto &o : this->occupied) {
      o = false;
    }
  }

  bool validate() const {
    if (this->accumulated_sequence.size() != this->config.pattern.size()) {
      return false;
    }
    return std::equal(this->accumulated_sequence.begin(),
                      this->accumulated_sequence.end(),
                      this->config.pattern.begin());
  }

private:
  config::code_point occupied[10];
  std::vector<config::code_point> accumulated_sequence;
};

struct EventData {
public:
  GHashTable *pointer_info; /* GdkDevice -> AxesInfo */
  GHashTable *touch_info;   /*GdkEventSequence -> AxesInfo */

  EventData();
};

static EventData *new_event_data();
static void event_data_free(EventData *data);
