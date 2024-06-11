// ! Helper functions, mainly for input validation

/// List of allowed input keys
pub const ALLOWED_KEYS: &'static [&str; 28] = &[
    "backspace",
    "enter",
    "+",
    "-",
    "*",
    "/",
    "=",
    "^",
    "(",
    ")",
    "!",
    "ln",
    "ce",
    "c",
    "×",
    "√",
    "÷",
    ".",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0"
];

/// Remaps button signs to real expression symbols
pub fn remap_keyboard_signs(value: &str) -> Vec<&str> {
    match value {
        "*" => vec!["×"],
        "÷" => vec!["/"],
        "x²" => vec!["^", "2"],
        "ln" => vec!["ln", "("],
        _ => vec![value],
    }
}

/// Checks if key is allowed input
pub fn is_legal_key(key: &str) -> bool {
    ALLOWED_KEYS.contains(&key)
}

/// Checks if string is legal operator
pub fn is_operator(value: &String) -> bool {
    let operators = vec!["+", "-", "×", "/", "!", "ln", "^", "√"];
    operators.iter().any(|&o| &*value == o)
}

/// Checks if string is valid number (only numeric characters or single dot followed by numeric characters)
pub fn is_number(value: &String) -> bool {
    value.len() > 0 && value.chars().all(|c| c == '.' || c.is_numeric())
}

