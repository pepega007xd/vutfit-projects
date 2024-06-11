//! This is the backend of the calculator.
//! It also handles parsing of mathematical expressions, which is implemented
//! in the module [parser].

#![cfg_attr(
    all(not(debug_assertions), target_os = "windows"),
    windows_subsystem = "windows"
)]

use tauri::{Manager, PhysicalSize, LogicalSize};

mod parser;
use parser::parse_input;

/// This function is the main entry point to the calculator app. It sets up the Tauri
/// runtime, and registers the `parse_and_eval` function to be used from the frontend.
fn main() {
    tauri::Builder::default()
        .setup(|app| {
            let main_window = app
                .get_window("main")
                .expect("Couldn't get the main window.");
            main_window
                .set_size(PhysicalSize {
                    height: 1100,
                    width: 700,
                })
                .expect("Couldn't change the windows dimensions.");
            main_window.set_title("Pawculator");
            Ok(())
        })
        .invoke_handler(tauri::generate_handler![parse_and_eval])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}

/// This function parses the input string, evaluates the resulting expression,
/// and returns the output as a string.
/// The output is returned as a Result containing either the result
/// or an error message in case of parsing or evaluation errors.
#[tauri::command]
fn parse_and_eval(input: &str) -> Result<String, String> {
    Ok(parse_input(input)
        .map_err(|e| format!("Parsing error: {}", e.to_string()))?
        .eval()
        .ok_or("Evaluation error: Invalid mathematical operation")?
        .normalize()
        .to_string())
}
