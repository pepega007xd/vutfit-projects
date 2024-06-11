<?php

namespace IPP\Student;

use DOMElement;

// All argument types must implement this interface
interface Argument
{
    // Accepts argument's element, returns parsed and validated argument
    public static function from_xml(DOMElement $element): Argument;
}
