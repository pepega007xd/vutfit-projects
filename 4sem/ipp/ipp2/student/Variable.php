<?php

namespace IPP\Student;

use DOMElement;

enum FrameType
{
    case Global;
    case Local;
    case Temporary;
}

// Represents a variable in the source program
class Variable implements Argument
{

    function __construct(
        public FrameType $frame,
        public string $name,
    ) {
    }

    // parses and validated XML representation of Variable
    public static function from_xml(DOMElement $element): Argument
    {
        if ($element->getAttribute("type") !== "var") {
            throw new InvalidXMLStructureException();
        }

        [$frame_type, $name] = explode("@", $element->textContent);

        $frame_type = match ($frame_type) {
            "GF" => FrameType::Global,
            "LF" => FrameType::Local,
            "TF" => FrameType::Temporary,
            default => throw new InvalidXMLStructureException()
        };

        return new Variable($frame_type, $name);
    }
}
