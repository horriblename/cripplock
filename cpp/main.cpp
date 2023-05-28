#include <gtk/gtk.h>
#include <iostream>
#include <main.hpp>

inline RuntimeData gAppState;

static gboolean event_cb(GtkWidget *widget, GdkEvent *event,
                         gpointer user_data) {
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

static EventData *new_event_data() {
  EventData *data;
  data = g_new0(EventData, 1);
  data->pointer_info =
      g_hash_table_new_full(NULL, NULL, NULL, (GDestroyNotify)g_free);
  data->touch_info =
      g_hash_table_new_full(NULL, NULL, NULL, (GDestroyNotify)g_free);

  // return data;
  return data;
}

static void event_data_free(EventData *data) {
  g_hash_table_destroy(data->pointer_info);
  g_hash_table_destroy(data->touch_info);
  g_free(data);
}

static void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *window;
  GtkWidget *label;

  window = gtk_application_window_new(app);

  auto event_box = gtk_event_box_new();
  gtk_container_add(GTK_CONTAINER(window), event_box);
  gtk_widget_set_support_multidevice(event_box, TRUE);
  gtk_widget_set_events(event_box, GDK_TOUCH_MASK | /* GDK_BUTTON_PRESS_MASK |
                                       GDK_BUTTON_RELEASE_MASK | */
                                       GDK_ENTER_NOTIFY_MASK);

  auto event_data = new_event_data();
  g_object_set_data_full(G_OBJECT(event_box),
                         "com.github.horriblename.cripplock", event_data,
                         (GDestroyNotify)event_data_free);

  g_signal_connect(event_box, "event", G_CALLBACK(event_cb), event_data);

  label = gtk_label_new("Hello GNOME!");
  gtk_container_add(GTK_CONTAINER(event_box), label);
  gtk_window_set_title(GTK_WINDOW(window), "Welcome to GNOME");
  gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);
  gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
  GtkApplication *app;
  int status;

  app = gtk_application_new(NULL, G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
