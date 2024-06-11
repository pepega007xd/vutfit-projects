fn main() {
    // if this directory doesn't exist, the `generate_context`
    // macro in src/main.rs will panic
    std::fs::create_dir("../frontend/dist").unwrap_or_default();
    tauri_build::build()
}
