use yew::{function_component, Html, html, Properties, classes, Callback};
use yewdux::{prelude::use_store};

use crate::{app::{AppState}, services::{state::{handle_interaction}}};

/// Color of the button
#[derive(Clone, Debug, PartialEq, Eq)]
pub enum ButtonType {
    Primary,
    Secondary,
    Blue,
    Action 
}

/// Default button type is secondary
impl Default for ButtonType {
    fn default() -> Self {
        Self::Secondary
    }
}

#[derive(Properties, Clone, PartialEq)]
pub struct Props {
    // The value of the button
    pub value: String,
    #[prop_or_default]
    // The type of the button
    pub button_type: ButtonType,
}

/// Maps the button type to the corresponding color
fn map_color(button_type: &ButtonType, darkmode: bool) -> &'static str {
    if darkmode {
       match button_type {
            ButtonType::Primary => "bg-slate-700",
            ButtonType::Secondary => "bg-gray-600",
            ButtonType::Blue => "bg-blue-500",
            ButtonType::Action => "bg-violet-500"
        }
    } else {
        match button_type {
            ButtonType::Primary => "bg-slate-400",
            ButtonType::Secondary => "bg-gray-100",
            ButtonType::Blue => "bg-blue-500",
            ButtonType::Action => "bg-amber-400"
        }
    }
}

fn map_text_color(darkmode:bool) -> &'static str {
    if darkmode {"text-zinc-300" } else { "text-black"}
}

/// Component for the keypad button
#[function_component(KeypadButton)]
pub fn keypad_button(props: &Props) -> Html {
    let (state, dispatch) = use_store::<AppState>();
    
    let bg_color = map_color(&props.button_type, state.dark_mode);
    let text_color = map_text_color(state.dark_mode);


    let onclick = {
        let props = props.clone();
        let state = state.clone();
        let dispatch = dispatch.clone();
        Callback::from(move |_| handle_interaction(&props.value, state.clone(), dispatch.clone()))
    };

    html! {
        <div 
            class={classes!(
                bg_color,
                "block",
                "flex", 
                "justify-center", 
                "items-center", 
                "rounded-lg", 
                "w-full",
                "text-4xl", 
                "h-full", 
                text_color,
                "font-semibold", 
                "select-none",
                "cursor-pointer",
                "drop-shadow-lg",
                "hover:opacity-80"
            )} 
            {onclick}
        >
            <span class={"keypad-button-text"}>{props.value.clone()}</span>
        </div>
    }
}