#include "../main.hpp"
#include "gdkmm/event.h"
#include "gtkmm/box.h"
#include "gtkmm/eventbox.h"
#include "gtkmm/label.h"
#include <gtkmm.h>
#include <iostream>
#include <memory>

inline std::unique_ptr<RuntimeData> gAppState;

using TouchEventCallback = std::function<bool(GdkEventTouch *)>;

TouchEventCallback mk_touch_event_cb(uint id) {
  return [&](GdkEventTouch *event) -> bool { return false; };
}

bool touch_event_cb(GdkEventTouch *event, short unsigned int id) {
  std::cout << "TOUCH EVENT[" << id << "]: ";
  switch (event->type) {
  case GDK_TOUCH_BEGIN:
    std::cout << "begin\n";
    gAppState->reset_sequence();
    gAppState->push_to_sequence(id);
    break;
  case GDK_TOUCH_UPDATE:
    std::cout << "update\n";
    gAppState->push_to_sequence(id);
    for (const auto &i : gAppState->accumulated_sequence) {
      std::cout << i;
    }
    std::cout << "\n";
    break;
  case GDK_TOUCH_END:
    std::cout << "end/";
    // fallthrough
  case GDK_TOUCH_CANCEL:
    std::cout << "cancel\n";
    if (gAppState->validate()) {
      gtk_main_quit();
      return TRUE;
    }
    gAppState->reset_sequence();
    break;
  default:
    std::cout << "something else\n";
  }
  // std::cout << event->sequence << " "
  //           << gdk_event_get_event_sequence((GdkEvent *)event) << "\n";
  return GDK_EVENT_PROPAGATE;
}

int main() {
  config::Config cfg = {std::vector<config::code_point>()};
  cfg.pattern.push_back(1);
  cfg.pattern.push_back(2);
  cfg.pattern.push_back(3);
  cfg.pattern.push_back(4);
  gAppState = std::make_unique<RuntimeData>(cfg);
  auto app = Gtk::Application::create();
  Gtk::Window window;
  app->signal_startup().connect([&] { app->add_window(window); });

  auto event_box = Gtk::EventBox();
  auto event_box2 = Gtk::EventBox();

  event_box.add_events(Gdk::TOUCH_MASK);
  event_box2.add_events(Gdk::TOUCH_MASK);

  event_box.signal_touch_event().connect(
      [&](GdkEventTouch *event) { return touch_event_cb(event, 1); });
  event_box2.signal_touch_event().connect(
      [&](GdkEventTouch *event) { return touch_event_cb(event, 2); });

  auto label = Gtk::Label("hello");
  auto label2 = Gtk::Label("world");
  event_box.add(label);
  event_box2.add(label2);

  auto box = Gtk::Box();
  box.add(event_box);
  box.add(event_box2);
  auto box_container = Gtk::EventBox();
  box_container.add_events(Gdk::TOUCH_MASK);
  box_container.add(box);
  box_container.signal_touch_event().connect([&](GdkEventTouch *event) {
    std::cout << "container received touch event\n";

    return true;
  });

  window.add(box_container);
  window.show_all();

  app->run();
  return 0;
}
