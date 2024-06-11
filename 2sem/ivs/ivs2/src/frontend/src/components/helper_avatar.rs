use yew::{function_component, Html, html, classes};
use yewdux::{prelude::use_store};

use crate::{app::{AppState}, services::state::{show_helper, show_help_page}};

const PAGES_COUNT: usize = 4;

/// Component for the helper avatar
#[function_component(HelperAvatar)]
pub fn helper_avatar() -> Html {
    let (state, dispatch) = use_store::<AppState>();

    let onclick = {
        let state = state.clone();
        move |_| {
            if state.help_page == PAGES_COUNT - 1 {
                dispatch.reduce_mut(|state| {
                    show_helper(state);
                    show_help_page(state, 0);   
                });
            } else{
                dispatch.reduce_mut(|state| show_help_page(state, state.help_page + 1));
            }
        }
    };

    html! {
        <div class={classes!("flex", "flex-col", "items-center","absolute","inset-0","z-10",)} {onclick}>
            <div class={classes!("fixed", "bottom-0", "left-0",)} >
                <img src={format!("static/page{}.png", state.help_page + 1)} width="600" height="300"/> 
            </div>
        </div>
    }
}