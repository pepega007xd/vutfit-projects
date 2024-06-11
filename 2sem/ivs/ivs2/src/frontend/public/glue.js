const invoke = window.__TAURI__.invoke;

export async function invokeParseAndEval(input) {
    return await invoke("parse_and_eval", {input: input});
}