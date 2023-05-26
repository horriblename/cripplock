use gtk::prelude::*;

mod config;

struct Pattern {
    pattern: Vec<u8>,
    sequence: Vec<u8>,
}

impl Pattern {
    fn new(pattern: Vec<u8>) -> Self {
        Pattern {
            pattern,
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
        self.pattern == self.sequence
    }
}

// https://github.com/wmww/gtk-layer-shell/blob/master/examples/simple-example.c
fn activate(application: &gtk::Application) {
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

    for i in 0..9_u8 {
        let row = i / 3;
        let col = i % 3;
        let id = i + 1;
        let label = gtk::Label::new(Some(&id.to_string()));
        let with_padding = gtk::Box::builder().margin(20).child(&label).build();

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
    application.connect_activate(|app| {
        activate(app);
    });

    application.run();
}
