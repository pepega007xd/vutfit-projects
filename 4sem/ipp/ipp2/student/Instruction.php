<?php

namespace IPP\Student;

use DOMElement;

// Represents a single instruction of the source program
class Instruction
{
    public string $opcode;
    public int $order;
    /** @var array<Argument> $arguments */
    public array $arguments = [];

    // parses element `instruction`, validates properties not validated by XML schema
    public function __construct(DOMElement $raw_instruction)
    {
        $this->opcode = strtoupper($raw_instruction->getAttribute("opcode"));
        $this->order = (int)$raw_instruction->getAttribute("order");

        $arg_types = InstructionHandler::get_arg_types($this->opcode);

        if ($raw_instruction->childElementCount !== count($arg_types)) {
            throw new InvalidXMLStructureException();
        }

        for ($i = 0; $i < count($arg_types); $i++) {
            // XML schema does not validate that the argument tag names are a continuous sequence
            $raw_arg = $raw_instruction->getElementsByTagName("arg" . ($i + 1))->item(0)
                ?? throw new InvalidXMLStructureException();

            if ($raw_arg instanceof DOMElement) {
                $raw_arg->textContent = trim($raw_arg->textContent);

                $parsed_arg = match ($arg_types[$i]) {
                    ArgumentType::Value => (match ($raw_arg->getAttribute("type")) {
                        // literal values are parsed differently than variables
                        "int", "bool", "string", "nil" => Value::from_xml($raw_arg),
                        "var" => Variable::from_xml($raw_arg),
                        default => throw new InvalidXMLStructureException()
                    }),
                    ArgumentType::Var => Variable::from_xml($raw_arg),
                    ArgumentType::Label => Label::from_xml($raw_arg),
                    ArgumentType::Type => InputType::from_xml($raw_arg),
                };

                array_push($this->arguments, $parsed_arg);
            }
        }
    }
}
