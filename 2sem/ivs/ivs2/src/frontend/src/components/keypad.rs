use yew::{function_component, Html, html, classes};
use crate::{components::keypad_button::{KeypadButton, ButtonType}};

#[function_component(Keypad)]
pub fn keypad() -> Html {
    html! {
        <div class={classes!("grow", "grid", "grid-cols-4", "gap-4", "w-full", "h-full")}>
            <KeypadButton value={"("} button_type={ButtonType::Primary} /> 
            <KeypadButton value={")"} button_type={ButtonType::Primary} /> 
            <KeypadButton value={"C"} button_type={ButtonType::Action} /> 
            <KeypadButton value={"CE"} button_type={ButtonType::Action} /> 

            <KeypadButton value={"x²"} button_type={ButtonType::Primary} /> 
            <KeypadButton value={"√"} button_type={ButtonType::Primary} /> 
            <KeypadButton value={"!"} button_type={ButtonType::Primary} /> 
            <KeypadButton value={"/"} button_type={ButtonType::Primary} /> 

            <KeypadButton value={"7"} /> 
            <KeypadButton value={"8"} /> 
            <KeypadButton value={"9"} /> 
            <KeypadButton value={"×"} button_type={ButtonType::Primary} /> 

            <KeypadButton value={"4"} /> 
            <KeypadButton value={"5"} /> 
            <KeypadButton value={"6"} /> 
            <KeypadButton value={"-"} button_type={ButtonType::Primary} /> 

            <KeypadButton value={"1"} /> 
            <KeypadButton value={"2"} /> 
            <KeypadButton value={"3"} /> 
            <KeypadButton value={"+"} button_type={ButtonType::Primary} /> 

            <KeypadButton value={"0"} /> 
            <KeypadButton value={"."} /> 
            <KeypadButton value={"ln"} /> 
            <KeypadButton value={"="} button_type={ButtonType::Blue} /> 
        </div>
    }
}