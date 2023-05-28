#include "gdkmm/event.h"
#include <gtkmm.h>
#include <iostream>

int main() {
  auto app = Gtk::Application::create();
  Gtk::Window window;
  app->signal_startup().connect([&] { app->add_window(window); });
  window.show();

  window.add_events(Gdk::TOUCH_MASK);
  window.signal_touch_event().connect([&](GdkEventTouch *event) -> bool {
    std::cout << "TOUCH EVENT: ";
    switch (event->type) {
    case GDK_TOUCH_BEGIN:
      std::cout << "begin\n";
      break;
    case GDK_TOUCH_UPDATE:
      std::cout << "update\n";
      break;
    case GDK_TOUCH_END:
      std::cout << "end\n";
      break;
    case GDK_TOUCH_CANCEL:
      std::cout << "cancel\n";
      break;
    default:
      std::cout << "something else\n";
    }
    std::cout << event->sequence << " "
              << gdk_event_get_event_sequence((GdkEvent *)event) << "\n";
    return GDK_EVENT_PROPAGATE;
  });
  window.signal_event().connect([&](GdkEvent *event) -> bool {
    std::cout << "EVENT: " << event->type << "\n";
    return GDK_EVENT_PROPAGATE;
  });
  app->run();
  return 0;
}
