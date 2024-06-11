<?php

namespace IPP\Student;

// Represents a memory frame, used inside Interpreter
class Frame
{
    /** @var array<string, ?Value> $content */
    private array $content = [];

    // Throws exception if variable is already defined
    function define_var(Variable $var): void
    {
        array_key_exists($var->name, $this->content)
            ? throw new SemanticErrorException("Redefintion of variable")
            : $this->content[$var->name] = null;
    }

    // Throws exception if variable is undefined,
    // returns null if variable is uninitialized (after DEFVAR)
    function get_value(Variable $var): ?Value
    {
        if (array_key_exists($var->name, $this->content)) {
            return $this->content[$var->name];
        } else {
            throw new UndefinedVariableException();
        }
    }

    // Throws exception if variable is undefined
    function set_value(Variable $var, Value $value): void
    {
        if (array_key_exists($var->name, $this->content)) {
            $this->content[$var->name] = $value;
        } else {
            throw new UndefinedVariableException();
        }
    }
}
