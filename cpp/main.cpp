#include "gdkmm/device.h"
#include "gdkmm/event.h"
#include "gtkmm/application.h"
#include "gtkmm/eventbox.h"
#include "gtkmm/label.h"
#include <gtk/gtk.h>
#include <gtkmm.h>
#include <iostream>
#include <main.hpp>

inline RuntimeData gAppState;

bool event_cb(GdkEventTouch *event) {
  if (event->type == GDK_TOUCH_BEGIN) {
    std::cout << "touch begin\n";
    gAppState.reset_sequence();
    gAppState.push_to_sequence(0);
  } else if (event->type == GDK_TOUCH_UPDATE) {
    std::cout << "touch update\n";
    gAppState.push_to_sequence(0);
  } else if (event->type == GDK_TOUCH_END) {
    std::cout << "touch end\n";
    if (gAppState.validate()) {
      gtk_main_quit();
      return TRUE;
    }
    gAppState.reset_sequence();
  } else if (event->type == GDK_ENTER_NOTIFY) {
    std::cout << "enter\n";
  } else if (event->type == GDK_BUTTON_PRESS) {
    std::cout << "button down\n";
  } else if (event->type == GDK_BUTTON_RELEASE) {
    std::cout << "button up\n";
  } else {
    std::cout << event->type << "\n";
  }

  return FALSE;
}

void activate(GtkApplication *app) {
  auto window = Gtk::Window();

  auto event_box = Gtk::EventBox();
  window.add(event_box);
  // window.set_support_multidevice(true);
  event_box.add_events(Gdk::TOUCH_MASK);

  window.signal_touch_event().connect(
      [&](GdkEventTouch *event) -> bool { return event_cb(event); });

  auto label = Gtk::Label("Hello");
  event_box.add(label);
  window.set_title("title");
  window.show_all();
}

int main(int argc, char **argv) {
  auto app = Gtk::Application::create("com.github.horriblename.cripplock");
  app->signal_activate().connect([app] { activate(&app); });
  app->run();
}
