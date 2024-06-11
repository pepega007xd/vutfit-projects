use yew::{function_component, Html, html, classes, };
use yewdux::{prelude::use_store};

use crate::{app::{AppState}, services::state::{ show_helper}};

/// Interactive help icon component
#[function_component(HelpIcon)]
pub fn help_icon() -> Html {
    let (state, dispatch) = use_store::<AppState>();

    // Handles click on the help icon
    let onclick = move |_| dispatch.reduce_mut(|state| show_helper(state));

    fn map_theme(is_darkmode:bool) ->  Vec<String> {
        if is_darkmode {
            vec![ "text-zinc-300".to_string() ]
        } else  {
            vec![ "text-zinc-700".to_string() ]
        }
    }

    let color_theme = map_theme(state.dark_mode);

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
        <span class={classes!("material-symbols-outlined", "text-red-500")}>
            {"support"}
        </span>
            <span class={"keypad-button-text"}>{"Pomoc"}</span>
            </div>
        </div>
    }
}