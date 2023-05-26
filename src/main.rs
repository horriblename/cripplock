use std::borrow::BorrowMut;
use std::cell::RefCell;
use std::rc::Rc;

use gtk::gdk;
use gtk::prelude::*;

mod config;

struct RuntimeData {
    config: config::Config,
    sequence: Vec<u8>,
}

impl RuntimeData {
    fn new(config: config::Config) -> Self {
        RuntimeData {
            config,
            sequence: Default::default(),
        }
    }

    fn handle_pattern_point(&mut self, id: u8) {
        if !self.sequence.contains(&id) {
            self.sequence.push(id);
        }
    }

    fn reset_sequence(&mut self) {
        self.sequence.clear()
    }

    /// Validate the built up sequence, returns true if the current sequence matches the required
    /// pattern
    fn validate(&self) -> bool {
        self.config.pattern == self.sequence
    }
}

// https://github.com/wmww/gtk-layer-shell/blob/master/examples/simple-example.c
fn activate(application: &gtk::Application, runtime_data: Rc<RefCell<RuntimeData>>) {
    // Create a normal GTK window however you like
    let window = gtk::ApplicationWindow::new(application);

    // Before the window is first realized, set it up to be a layer surface
    gtk_layer_shell::init_for_window(&window);

    // Display it above normal windows
    gtk_layer_shell::set_layer(&window, gtk_layer_shell::Layer::Overlay);

    // The margins are the gaps around the window's edges
    // Margins and anchors can be set like this...
    // gtk_layer_shell::set_margin(&window, gtk_layer_shell::Edge::Left, 40);
    // gtk_layer_shell::set_margin(&window, gtk_layer_shell::Edge::Right, 40);
    // gtk_layer_shell::set_margin(&window, gtk_layer_shell::Edge::Top, 20);

    // ... or like this
    // Anchors are if the window is pinned to each edge of the output
    let anchors = [
        (gtk_layer_shell::Edge::Left, false),
        (gtk_layer_shell::Edge::Right, true),
        (gtk_layer_shell::Edge::Top, true),
        (gtk_layer_shell::Edge::Bottom, true),
    ];

    for (anchor, state) in anchors {
        gtk_layer_shell::set_anchor(&window, anchor, state);
    }

    // Set up a widget
    let label = gtk::Label::new(Some(""));
    label.set_markup("<span font_desc=\"20.0\">GTK Layer Shell example!</span>");

    let grid = gtk::Grid::new();

    // TODO
    // let mut pattern_acc = Rc::new(Pattern::new(vec![1, 2, 3, 4, 7]));

    for i in 0..9_u8 {
        let row = i / 3;
        let col = i % 3;
        let id = i + 1;
        let label = gtk::Label::new(Some(&id.to_string()));
        let event_box = gtk::EventBox::builder().child(&label).build();
        let runtime_data_clone = Rc::clone(&runtime_data);
        event_box.connect_touch_event(move |_, event| {
            println!("received touch event! {:?}", event.event_type());
            match event.event_type() {
                gdk::EventType::TouchBegin => {
                    let mut runtime_data = runtime_data_clone.as_ref().borrow_mut();
                    runtime_data.reset_sequence();
                    runtime_data.handle_pattern_point(id);
                }
                gdk::EventType::TouchUpdate => runtime_data_clone
                    .as_ref()
                    .borrow_mut()
                    .handle_pattern_point(id),
                gdk::EventType::TouchEnd => {
                    let mut runtime_data = runtime_data_clone.as_ref().borrow_mut();
                    println!("{:?}", runtime_data.sequence);
                    if runtime_data.validate() {
                        gtk::main_quit();
                    }
                    runtime_data.reset_sequence();
                }
                gdk::EventType::TouchCancel => {
                    runtime_data_clone.as_ref().borrow_mut().reset_sequence()
                }
                _ => unreachable!(),
            }
            Inhibit(true)
        });
        let with_padding = gtk::Box::builder().margin(20).child(&event_box).build();

        grid.attach(&with_padding, col.into(), row.into(), 1, 1);
    }

    window.add(&grid);

    window.set_border_width(12);
    window.show_all()
}

fn main() {
    let application = gtk::Application::new(
        Some("com.github.horriblename.cripplock"),
        Default::default(),
    );
    // let config = config::read_config();
    //
    let runtime_data = Rc::new(RefCell::new(RuntimeData::new(config::Config {
        pattern: vec![1, 2, 3, 4, 7],
    })));
    application.connect_activate(move |app| {
        let runtime_data = runtime_data.clone();
        activate(app, runtime_data);
    });

    application.run();
}
