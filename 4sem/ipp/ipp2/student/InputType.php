<?php

namespace IPP\Student;

use DOMElement;

// Represents the argument of TYPE instruction,
// which determines the type of variable being loaded
enum InputType implements Argument
{
    case int;
    case string;
    case bool;

    public static function from_xml(DOMElement $element): Argument
    {
        if ($element->getAttribute("type") !== "type") {
            throw new InvalidXMLStructureException();
        }

        return match ($element->textContent) {
            "int" => InputType::int,
            "string" => InputType::string,
            "bool" => InputType::bool,
            default => throw new InvalidXMLStructureException()
        };
    }
}
