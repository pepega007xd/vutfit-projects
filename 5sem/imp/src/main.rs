mod display;
mod gui;
mod input;
mod state;
mod tuner;

use esp_idf_svc::{hal::prelude::Peripherals, nvs::EspDefaultNvsPartition};
use std::sync::mpsc::channel;

use display::setup_display;
use input::{spawn_button_listener, spawn_encoder_listener};
use tuner::spawn_tuner_thread;

/// All possible inputs to be processed in the event loop.
///
/// The events are generated by user actions and by the tuner module itself.
#[derive(Clone, Debug, PartialEq, Eq)]
enum InputEvent {
    /// Short (< 600ms) press of the rotary encoder button
    ShortPress,
    /// Long (> 600ms) press of the rotary encoder button
    LongPress,
    /// Counterclockwise turn of the rotary encoder
    ScrollDown,
    /// Clockwise turn of the rotary encoder
    ScrollUp,
    /// Frequency of the tuner was changed during seeking
    ChangeFrequency(u32),
    /// Station info changed
    ChangeStationInfo(String),
    /// RSSI value changed (changes are debounced)
    ChangeRSSI(u8),
}

/// All possible actions sent to the tuner.
enum OutputCommand {
    SetFrequency(u32),
    SetVolume(u8),
    /// Seek with increasing frequency
    SeekUp,
    /// Seek with decreasing frequency
    SeekDown,
}

/// This enum represents all UI elements to be drawn to the screen.
#[derive(Clone, Copy, PartialEq, Eq)]
enum UIElement {
    SeekDown,
    FreqControl,
    SeekUp,
    /// Preset button with its index
    Preset(u8),
    VolumeControl,
}

/// Number of preset stations available to the user.
const NUM_PRESETS: u8 = 4;

/// This struct holds the current state of the whole application.
struct AppState {
    /// Currently tuned frequency, updated through
    /// GUI by the user and by the tuner during seek.
    freq_khz: u32,

    volume: u8,

    /// Current station info (Radio Text from RDS),
    /// updated by the tuner.
    station_info: String,

    rssi: u8,

    /// UI element, on which the cursor is currently located
    cursor_at: UIElement,

    /// Whether the current UI element is selected for manipulation
    /// (valid only for frequency and volume control, otherwise always false)
    element_is_active: bool,
}

fn main() {
    // It is necessary to call this function once. Otherwise some patches to the runtime
    // implemented by esp-idf-sys might not link properly. See https://github.com/esp-rs/esp-idf-template/issues/71
    esp_idf_svc::sys::link_patches();

    // Bind the log crate to the ESP Logging facilities
    esp_idf_svc::log::EspLogger::initialize_default();

    // initialize nonvolatile storage
    let mut nvs =
        esp_idf_svc::nvs::EspNvs::new(EspDefaultNvsPartition::take().unwrap(), "namespace", true)
            .unwrap();

    // initialize GPIO
    let peripherals = Peripherals::take().unwrap();

    // create channels for sending inputs and outputs
    let (event_sender, event_receiver) = channel::<InputEvent>();
    let (command_sender, command_receiver) = channel::<OutputCommand>();

    // setup listener for button presses
    spawn_button_listener(peripherals.pins.gpio17, event_sender.clone());

    // setup listener for rotary encoder inputs
    spawn_encoder_listener(
        peripherals.pins.gpio25,
        peripherals.pins.gpio26,
        event_sender.clone(),
    );

    // setup RDA5807M tuner
    spawn_tuner_thread(
        peripherals.i2c0,
        peripherals.pins.gpio21,
        peripherals.pins.gpio22,
        event_sender,
        command_receiver,
    );

    // setup SSD1306 display
    let mut display = setup_display(
        peripherals.spi3,
        peripherals.pins.gpio18,
        peripherals.pins.gpio23,
        peripherals.pins.gpio13,
        peripherals.pins.gpio12,
    );

    // initialize application state
    let mut state = AppState::new();

    // draw GUI
    state.update_ui(&mut display).unwrap();

    // event loop - wait for next input event, process it, and update GUI
    while let Ok(event) = event_receiver.recv() {
        state.process_event(event, &command_sender, &mut nvs);
        state.update_ui(&mut display).unwrap();
    }
}
