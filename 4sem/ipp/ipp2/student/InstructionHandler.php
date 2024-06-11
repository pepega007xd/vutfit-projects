<?php

namespace IPP\Student;

use IPP\Core\Interface\InputReader;
use IPP\Core\Interface\OutputWriter;

// Represents the expected type of argument for an instruction.
// Note that `Value` can be either variable or literal value in the source program, but Interpreter->run_program
// will first get the value of the variable, and this value will be passed into callback from get_handler
enum ArgumentType
{
    case Var;
    case Value;
    case Type;
    case Label;
}

class InstructionHandler
{
    public function __construct(
        private Interpreter $state,
        private InputReader $stdin_reader,
        private OutputWriter $stdout_writer,
        private OutputWriter $stderr_writer
    ) {
    }

    /** @return array<ArgumentType> */
    public static function get_arg_types(string $opcode): array
    {
        return match ($opcode) {
            "MOVE" => [ArgumentType::Var, ArgumentType::Value],
            "CREATEFRAME" => [],
            "PUSHFRAME" => [],
            "POPFRAME" => [],
            "DEFVAR" => [ArgumentType::Var],
            "CALL" => [ArgumentType::Label],
            "RETURN" => [],
            "PUSHS" => [ArgumentType::Value],
            "POPS" => [ArgumentType::Var],
            "ADD" => [ArgumentType::Var, ArgumentType::Value, ArgumentType::Value],
            "SUB" => [ArgumentType::Var, ArgumentType::Value, ArgumentType::Value],
            "MUL" => [ArgumentType::Var, ArgumentType::Value, ArgumentType::Value],
            "IDIV" => [ArgumentType::Var, ArgumentType::Value, ArgumentType::Value],
            "LT" => [ArgumentType::Var, ArgumentType::Value, ArgumentType::Value],
            "GT" => [ArgumentType::Var, ArgumentType::Value, ArgumentType::Value],
            "EQ" => [ArgumentType::Var, ArgumentType::Value, ArgumentType::Value],
            "AND" => [ArgumentType::Var, ArgumentType::Value, ArgumentType::Value],
            "OR" => [ArgumentType::Var, ArgumentType::Value, ArgumentType::Value],
            "NOT" => [ArgumentType::Var, ArgumentType::Value],
            "INT2CHAR" => [ArgumentType::Var, ArgumentType::Value],
            "STRI2INT" => [ArgumentType::Var, ArgumentType::Value, ArgumentType::Value],
            "READ" => [ArgumentType::Var, ArgumentType::Type],
            "WRITE" => [ArgumentType::Value],
            "CONCAT" => [ArgumentType::Var, ArgumentType::Value, ArgumentType::Value],
            "STRLEN" => [ArgumentType::Var, ArgumentType::Value],
            "GETCHAR" => [ArgumentType::Var, ArgumentType::Value, ArgumentType::Value],
            "SETCHAR" => [ArgumentType::Var, ArgumentType::Value, ArgumentType::Value],
            "TYPE" => [ArgumentType::Var, ArgumentType::Value],
            "LABEL" => [ArgumentType::Label],
            "JUMP" => [ArgumentType::Label],
            "JUMPIFEQ" => [ArgumentType::Label, ArgumentType::Value, ArgumentType::Value],
            "JUMPIFNEQ" => [ArgumentType::Label, ArgumentType::Value, ArgumentType::Value],
            "EXIT" => [ArgumentType::Value],
            "DPRINT" => [ArgumentType::Value],
            "BREAK" => [],
            default => throw new SemanticErrorException("Invalid opcode")
        };
    }

    public function get_handler(string $opcode): callable
    {
        $state = $this->state;

        return match ($opcode) {
            "MOVE" => fn ($var, $value) => $state->set_value($var, $value),
            "CREATEFRAME" => fn () => $state->create_frame(),
            "PUSHFRAME" => fn () => $state->push_frame(),
            "POPFRAME" => fn () => $state->pop_frame(),
            "DEFVAR" => fn ($var) => $state->define_var($var),
            "CALL" => fn ($label) => $state->call($label),
            "RETURN" => fn () => $state->return(),
            "PUSHS" => fn ($value) => $state->stack_push($value),
            "POPS" => fn ($var) => $state->stack_pop($var),
            "ADD" => fn ($var, $lhs, $rhs) => $state->set_value($var, $lhs->add($rhs)),
            "SUB" => fn ($var, $lhs, $rhs) => $state->set_value($var, $lhs->sub($rhs)),
            "MUL" => fn ($var, $lhs, $rhs) => $state->set_value($var, $lhs->mul($rhs)),
            "IDIV" => fn ($var, $lhs, $rhs) => $state->set_value($var, $lhs->idiv($rhs)),
            "LT" => fn ($var, $lhs, $rhs) => $state->set_value($var, $lhs->lt($rhs)),
            // reverse order RHS < LHS
            "GT" => fn ($var, $lhs, $rhs) => $state->set_value($var, $rhs->lt($lhs)),
            "EQ" => fn ($var, $lhs, $rhs) => $state->set_value($var, $lhs->eq($rhs)),
            "AND" => fn ($var, $lhs, $rhs) => $state->set_value($var, $lhs->and($rhs)),
            "OR" => fn ($var, $lhs, $rhs) => $state->set_value($var, $lhs->or($rhs)),
            "NOT" => fn ($var, $value) => $state->set_value($var, $value->not()),
            "INT2CHAR" => fn ($var, $value) => $state->set_value($var, $value->int2char()),
            "STRI2INT" => fn ($var, $str, $idx) => $state->set_value($var, $str->stri2int($idx)),
            "READ" => fn ($var, $type) => $state->set_value($var, Value::read($this->stdin_reader, $type)),
            "WRITE" => fn ($value) => $value->write($this->stdout_writer),
            "CONCAT" => fn ($var, $lhs, $rhs) => $state->set_value($var, $lhs->concat($rhs)),
            "STRLEN" => fn ($var, $value) => $state->set_value($var, $value->strlen()),
            "GETCHAR" => fn ($var, $str, $idx) => $state->set_value($var, $str->getchar($idx)),
            "SETCHAR" => fn ($var, $idx, $char) => $state->set_value($var, $state->get_value($var)->setchar($idx, $char)),
            // in this case, `Value` ca, refer to uninitialized Variable, and $value can therefore
            // be null, this is handled by a special case in Interpreter->run_program
            "TYPE" => fn ($var, $value) => $state->set_value($var, Value::type($value)),
            // no-op, $label is here to stop linter from complaining about unused parameter
            "LABEL" => fn ($label) => $label,
            "JUMP" => fn ($label) => $state->jump($label),
            "JUMPIFEQ" => fn ($label, $lhs, $rhs) => $lhs->eq($rhs)->get_bool_value()
                ? $state->jump($label)
                : null,
            "JUMPIFNEQ" => fn ($label, $lhs, $rhs) => $lhs->eq($rhs)->get_bool_value()
                ? null
                : $state->jump($label),
            "EXIT" => fn ($value) => $value->exit(),
            "DPRINT" => fn ($value) => $value->dprint($this->stderr_writer),
            "BREAK" => fn () => $state->break($this->stderr_writer),
            default => throw new SemanticErrorException("Invalid opcode")
        };
    }
}
