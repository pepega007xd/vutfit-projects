use yew::{function_component, Html, html, classes};
use yewdux::{prelude::use_store};

use crate::{app::{AppState}, services::state::{switch_theme}};

fn map_theme(is_darkmode :bool) ->  Vec<String> {
    if is_darkmode {
        vec![ "text-zinc-300".to_string() ]
    } else  {
        vec![ "text-zinc-700".to_string() ]
    }
}

fn map_text(is_darkmode:bool) -> &'static str {
    if is_darkmode { "Dark" } else { "Light" }
}

/// Component for the theme switcher
#[function_component(ThemeSwitcher)]
pub fn theme_switcher() -> Html {
    let (state, dispatch) = use_store::<AppState>();

    let onclick = move |_| dispatch.reduce_mut(|state| switch_theme(state));

    let color_theme = map_theme(state.dark_mode);
    let button_text = map_text(state.dark_mode);

    html! {
        <div 
            class={classes!(
                color_theme,
                "w-24",
                "h-24", 
                "flex", 
                "justify-center", 
                "items-center", 
                "focus:outline-none", 
                "focus:shadow-outline",
                "select-none",
                "cursor-pointer",
                "opacity-70",
                "rounded-xl", 
                "hover:opacity-80"
            )} 
            {onclick}
        >
        <div class={classes!("flex", "flex-col", "items-center",)}>
            <span class="material-symbols-outlined">
            {"dark_mode"}
            </span>
            
            <span class={"keypad-button-text"}>{button_text}</span>
            
            </div>
        </div>
    }
}