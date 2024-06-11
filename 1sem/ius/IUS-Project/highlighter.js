const fs = require("fs");

const INPUT_FILE_PATH = "./class.drawio";
const OUTPUT_FILE_PATH = "./class.highlighted.drawio";

const file = fs.readFileSync(INPUT_FILE_PATH, "utf8");

const COLOR = {
	COMMENT: "#888888",
	REGEX: "#e46f6f",
	NUMBER: "#c88dff",
	STRING: "#ffe484",
	TEMPLATE_CODE: "#71b0ff",
	NAME: "#c0ff00",
	DATA_TYPE: "#00ffc4",
	IMPORT: "#ed0082",
	CONTROL: "#ed0082",
	CONSTANT: "#ff0073",
	CONDITIONAL: "#ed0082",
	FUNCTION: "#ff904d",
	ARITHMETIC: "#ffffff",
	STORAGE: "#5ea6e8",
	KEYWORD: "#ed0082",
	VARIABLE: "#b9e7ff",
	ENUM: "#58c5d8",
	INDENT: "#505050",
	SELECTION: "#0089ac",
	SELECTED_LINE: "#353535",
	LINE_NUMBER: "#808080",
	BACKGROUND: "#262626"
};

function escapeHTML(text) {
	return text
		.replace(/&/g, "&amp;")
		.replace(/</g, "&lt;")
		.replace(/>/g, "&gt;")
		.replace(/"/g, "&quot;")
		.replace(/'/g, "&#039;");
}

function highlight(text, color) {
	return escapeHTML(`<span style="color:${color}">${text}</span>`);
}

function highlightType(type) {
	return type.trim()
		.replace(/&lt;span&gt;/g, "") //Remove this invalid tag (no clue why it's present)
		.replace(/(?<=;|\?|^|\s)(?:([a-z]\w*)|([A-Z]\w*))(?=&|\?|$|\s)/gm, (m, primitive, instance) => {
			if(primitive) return highlight(primitive, COLOR.DATA_TYPE);
			if(instance && instance.trim() == "extends") return highlight(instance, COLOR.STORAGE);
			if(instance) return highlight(instance, COLOR.NAME);
		});
}

const highlighted = file.replace(/&gt;\s*([+\-]?)\s*(?:&amp;lt;([\w\s&;?]+)&amp;gt;)?\s*(\w+?)\s*(?:(\()(.*?)\))?\s*:\s*([\w\s&;?]+?)\s*&lt;/gmi, (m, access, generic, name, isMethod, params, type) => {
	if(access) access = highlight(access, COLOR.STORAGE);
	if(generic) generic = `&amp;lt;${highlightType(generic)}&amp;gt;`;
	if(name) name = highlight(name, isMethod ? COLOR.FUNCTION : COLOR.VARIABLE);
	if(params) params = params.split(",").map(param => {
		const [name, type] = param.split(":");
		return `${highlight(name, COLOR.VARIABLE)}: ${highlightType(type)}`;
	}).join(highlight(",", COLOR.ARITHMETIC) + " ");
	if(type) type = highlightType(type);

	if(isMethod) return `&gt; ${access || ""} ${generic || ""} ${name}(${params || ""}): ${type} &lt;`;
	else return `&gt; ${access || ""} ${name}: ${type} &lt;`;
}).replace(/(@template)\s+([\w\s,])/g, (m, keyword, types) => {
	keyword = highlight(keyword, COLOR.STORAGE);
	types = types.split(",").map(type => {
		return highlight(type.trim(), COLOR.NAME);
	}).join(highlight(",", COLOR.ARITHMETIC) + " ");

	return `${keyword} ${types}`;
});

fs.writeFileSync(OUTPUT_FILE_PATH, highlighted);
//fs.writeFileSync(OUTPUT_FILE_PATH + ".xml", highlighted);