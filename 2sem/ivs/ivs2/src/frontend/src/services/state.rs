//! Module for state management
use std::rc::Rc;

use rand;
use rand::Rng;
use wasm_bindgen_futures::spawn_local;
use yewdux::prelude::{Dispatch};

use crate::{app::AppState, parse_and_eval};

use super::utils::{is_number, remap_keyboard_signs, is_legal_key};

/// Add array of values value to an expression
pub fn expression_add_many(state: &mut AppState, values: Vec<&str>) {
    for value in values.into_iter() {
        expression_add(state, value);
    }
}

/// Add value to an expression
pub fn expression_add(state: &mut AppState, value: &str) {
    let last_term = state.expression.last();

    if last_term.is_none() {
        state.expression.push(value.to_owned());
        return;
    }

    let last_term = last_term.unwrap();

    if is_number(&value.to_owned()) && is_number(last_term) {
        if let Some(mut last_item) = state.expression.pop() {
            // If last term is a number, and current term is number too, we concatenate them
            last_item.push_str(&value);
            state.expression.push(last_item);
        }
    } else {
        state.expression.push(value.to_owned());
    }
}

/// Remove last item from expression
pub fn expression_pop(state: &mut AppState) {
    if state.expression.len() == 0 || state.expression.last().unwrap().len() == 0 {
        state.expression.pop();
        return;
    }

    let last_term = state.expression.last().unwrap();

    if is_number(last_term) {
        if let Some(mut last_item) = state.expression.pop() {
            last_item.pop();
            if last_item.len() == 0 {
                return;
            }

            state.expression.push(last_item);
        }
    } else {
        state.expression.pop();
    }
}

/// Clear expression
pub fn expression_clear(state: &mut AppState) {
    state.expression = vec![];
}

/// Change color theme
pub fn switch_theme(state: &mut AppState) {
    state.dark_mode = !state.dark_mode;
}

/// Show/hide helper
pub fn show_helper(state: &mut AppState) {
    state.show_helper = !state.show_helper;
}

/// Show help page with given index
pub fn show_help_page(state: &mut AppState, page: usize) {
    state.help_page = page;
}

/// 
pub fn handle_interaction(value: &str, state: Rc<AppState>, dispatch: Dispatch<AppState>) {
    // Clear errors from previous evaluation
    if state.expression == vec!["error"]  {
        dispatch.reduce_mut(|s| {
            s.expression = vec![];
        });
    }

    // Checks if input is a allow key
    if !is_legal_key(&value.to_lowercase()) {
        return;
    }

    match value {
        "C" => dispatch.reduce_mut(expression_pop),
        "CE" => dispatch.reduce_mut(|state| {
            state.result = vec![];
            expression_clear(state);
        }),
        "backspace" => {
            dispatch.reduce_mut(|state| expression_pop(state));
        },
        "=" | "enter" => {
            let state = state.clone();
            let dispatch = dispatch.clone();

            // Secret feature
            if state.expression == vec!["1", "+", "+"] {
                dispatch.reduce_mut(|s| {
                    let mut rng = rand::thread_rng();
                    let effective_size = rng.gen_range(0..50);
                    s.result = vec![
                        String::from_utf8(
                            vec![0x5, 0x41, 0x4b, 0x59, 0x5a, 0x4b, 0x58, 0x4f, 0x41]
                            .iter()
                            .map(|x| x ^ 0x2a)
                            .collect()
                        ).unwrap()
                    ];
                    s.expression = vec![format!("{}", effective_size)];
                });
                return;
            }

            // Spawn a new thread to evaluate the expression
            spawn_local(async move {
                match parse_and_eval(&state.expression.join(" ")).await {
                    Ok(result) => {
                        let result = result.as_string().unwrap();
                        dispatch.reduce_mut(|s| {
                            s.result = s.expression.clone();
                            expression_clear(s);
                            expression_add_many(s, result.split(" ").collect());
                        });
                    },
                    Err(_) => {
                        dispatch.reduce_mut(|state| {
                            state.expression = vec!["error".to_owned()];
                            state.result= vec![];
                        })
                    }
                };
            });
        },
        _ => dispatch.reduce_mut(|state| expression_add_many(state, remap_keyboard_signs(value)))
    }
}