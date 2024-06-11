use std::rc::Rc;
use std::vec;

use gloo::events::EventListener;
use wasm_bindgen::{JsCast, UnwrapThrowExt};
use web_sys::{KeyboardEvent};
use yew::{function_component, Html, html, classes, Callback, use_effect};
use yewdux::prelude::use_store;
use yewdux::store::Store;

use crate::components::keypad::{Keypad};
use crate::components::display::{Display};
use crate::components::theme_switcher::{ThemeSwitcher};
use crate::components::help_icon::{HelpIcon};
use crate::components::helper_avatar::{HelperAvatar};

use crate::services::state::{handle_interaction};

#[derive(Debug, Clone, PartialEq, Eq, Store)]
pub struct AppState{
    /// The expression to be evaluated
    pub expression: Vec<String>,
    /// The result of the expression
    pub result: Vec<String> ,
    /// The theme of the app
    pub dark_mode: bool ,
    /// Toggle the helper avatar
    pub show_helper: bool ,
    /// The current number of the help page
    pub help_page : usize,
}

impl Default for AppState {
    fn default() -> Self {
        Self { expression: vec![], result: vec![], dark_mode: true, show_helper: false, help_page:0  }
    }
}

#[function_component(App)]
pub fn app() -> Html {
    let (state, dispatch) = use_store::<AppState>();

    let document = web_sys::window().unwrap().document().unwrap();
    let color_theme = map_theme(state.dark_mode);

    // Register keydown event listener on every re-render of the component
    use_effect({
        let state: Rc<AppState> = state.clone();

        move || {
            let onkeydown = {
                Callback::from(move |ev: KeyboardEvent| handle_interaction(&ev.key().to_lowercase(), state.clone(), dispatch.clone()))
            };

            // Listen to keydown events
            let listener = EventListener::new(&document, "keydown", move |e| {
                let e = e.dyn_ref::<web_sys::KeyboardEvent>().unwrap_throw();
                onkeydown.emit(e.clone())
            });

            move || drop(listener)
        }
    });


    fn map_theme(is_darkmode:bool) ->  Vec<String> {
        if is_darkmode {
            vec![ "from-slate-800".to_owned(), "to-app-bg-end".to_owned()] 
        } else {
            vec![ "from-slate-100".to_owned(), "to-violet-300".to_owned()] 
        }
    }

    html! {
        <div class={classes!("app", "bg-gradient-to-b", color_theme, "h-screen", "p-5", "flex", "flex-col")}>
            <div class={classes!("flex-none", "flex", "justify-between","items-start", "py-5")}>
                <HelpIcon/>
                <ThemeSwitcher/>
            </div>
            <Display/>
            <Keypad/>
            if state.show_helper {
                <HelperAvatar/>
            }
        </div>
    }
}