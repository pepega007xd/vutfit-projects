<?php

namespace IPP\Student;

use DOMElement;

class Label implements Argument
{
    function __construct(public string $name)
    {
    }

    public static function from_xml(DOMElement $element): Argument
    {
        if ($element->getAttribute("type") !== "label") {
            throw new InvalidXMLStructureException();
        }

        return new Label($element->textContent);
    }
}
