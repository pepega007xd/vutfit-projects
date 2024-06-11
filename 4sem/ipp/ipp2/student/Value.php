<?php

namespace IPP\Student;

use DOMElement;
use IPP\Core\Interface\InputReader;
use IPP\Core\Interface\OutputWriter;

// Represents the type of Value
enum ValueType
{
    case int;
    case bool;
    case string;
    case nil;
}

// shortcuts for more readable code
const Int = ValueType::int;
const Bool = ValueType::bool;
const String = ValueType::string;
const Nil = ValueType::nil;

// Represents a concrete value inside interpreter, along with its type,
// public interface implements most arithmetic and relational instructions,
// all methods return new instances of Value, the original value is never modified
class Value implements Argument
{
    public function __construct(
        private ValueType $type,

        private bool $bool_value = false,
        private int $int_value = 0,
        private string $string_value = ""
    ) {
    }

    static function bool_value(bool $value): Value
    {
        return new Value(Bool, bool_value: $value);
    }

    static function int_value(int $value): Value
    {
        return new Value(Int, int_value: $value);
    }

    static function string_value(string $value): Value
    {
        return new Value(String, string_value: $value);
    }

    static function nil_value(): Value
    {
        return new Value(Nil);
    }

    // for use in conditional jump instructions
    public function get_bool_value(): bool
    {
        return match ($this->type) {
            Bool => $this->bool_value,
            default => throw new WrongTypeException()
        };
    }

    public function add(Value $other): Value
    {
        return match ([$this->type, $other->type]) {
            [Int, Int] => Value::int_value($this->int_value + $other->int_value),
            default => throw new WrongTypeException()
        };
    }


    public function sub(Value $other): Value
    {
        return match ([$this->type, $other->type]) {
            [Int, Int] => Value::int_value($this->int_value - $other->int_value),
            default => throw new WrongTypeException()
        };
    }

    public function mul(Value $other): Value
    {
        return match ([$this->type, $other->type]) {
            [Int, Int] => Value::int_value($this->int_value * $other->int_value),
            default => throw new WrongTypeException()
        };
    }

    public function idiv(Value $other): Value
    {
        // check for division by zero
        if ($other->type === Int && $other->int_value === 0) {
            throw new WrongValueException();
        }

        return match ([$this->type, $other->type]) {
            [Int, Int] => Value::int_value(intdiv($this->int_value, $other->int_value)),
            default => throw new WrongTypeException()
        };
    }

    // gt can be implemented as lt with reverse argument order
    public function lt(Value $other): Value
    {
        return match ([$this->type, $other->type]) {
            [Int, Int] => Value::bool_value($this->int_value < $other->int_value),
            [Bool, Bool] => Value::bool_value($this->bool_value < $other->bool_value),
            [String, String] => Value::bool_value(strcmp($this->string_value, $other->string_value) === -1),
            default => throw new WrongTypeException()
        };
    }

    public function eq(Value $other): Value
    {
        return match ([$this->type, $other->type]) {
            [Int, Int] => Value::bool_value($this->int_value === $other->int_value),
            [Bool, Bool] => Value::bool_value($this->bool_value === $other->bool_value),
            [String, String] => Value::bool_value(strcmp($this->string_value, $other->string_value) === 0),
            // comparing a non-nil value with nil is valud and returns false
            [Int, Nil], [Bool, Nil], [String, Nil],
            [Nil, Int], [Nil, Bool], [Nil, String] => Value::bool_value(false),
            [Nil, Nil] => Value::bool_value(true),
            default => throw new WrongTypeException()
        };
    }

    public function and(Value $other): Value
    {
        return match ([$this->type, $other->type]) {
            [Bool, Bool] => Value::bool_value($this->bool_value && $other->bool_value),
            default => throw new WrongTypeException()
        };
    }

    public function or(Value $other): Value
    {
        return match ([$this->type, $other->type]) {
            [Bool, Bool] => Value::bool_value($this->bool_value || $other->bool_value),
            default => throw new WrongTypeException()
        };
    }

    public function not(): Value
    {
        return match ($this->type) {
            Bool => Value::bool_value(!$this->bool_value),
            default => throw new WrongTypeException()
        };
    }

    public function int2char(): Value
    {
        return match ($this->type) {
            Int => Value::string_value(mb_chr($this->int_value)
                ?: throw new StringOperationException("Value is not a valid Unicode codepoint")),
            default => throw new WrongTypeException()
        };
    }

    public function stri2int(Value $index): Value
    {
        // already bounds-checked
        return Value::int_value(mb_ord(
            $this->getchar($index)->string_value
        ));
    }

    public static function read(InputReader $stdin, InputType $type): Value
    {
        $value = match ($type) {
            InputType::int => $stdin->readInt(),
            InputType::bool => $stdin->readBool(),
            InputType::string => $stdin->readString(),
        };

        if ($value === null) {
            return Value::nil_value();
        }

        return match (true) {
            is_int($value) => Value::int_value($value),
            is_bool($value) => Value::bool_value($value),
            is_string($value) => Value::string_value($value),
        };
    }

    public function write(OutputWriter $stdout): void
    {
        match ($this->type) {
            ValueType::int => $stdout->writeInt($this->int_value),
            ValueType::string => $stdout->writeString($this->string_value),
            ValueType::bool => $stdout->writeBool($this->bool_value),
            ValueType::nil => null,
        };
    }

    public function concat(Value $other): Value
    {
        return match ([$this->type, $other->type]) {
            [String, String] => Value::string_value($this->string_value . $other->string_value),
            default => throw new WrongTypeException()
        };
    }

    public function strlen(): Value
    {
        return match ($this->type) {
            String => Value::int_value(mb_strlen($this->string_value)),
            default => throw new WrongTypeException()
        };
    }

    public function getchar(Value $index): Value
    {
        return match ([$this->type, $index->type]) {
            [String, Int] => Value::string_value(
                mb_substr($this->string_value, $index->int_value, 1)
                    ?: throw new StringOperationException("Character index is out of bounds")
            ),
            default => throw new WrongTypeException()
        };
    }

    public function setchar(Value $index, Value $replacement): Value
    {
        // already bounds-checked
        $replacement_char = $replacement->getchar(Value::int_value(0))->string_value;

        if ($index->type !== Int) {
            throw new WrongTypeException();
        }

        // why is mb_substr_replace missing from the standard library ???
        $orig_string = $this->string_value;
        $index = $index->int_value;

        if (mb_strlen($orig_string) <= $index) {
            throw new StringOperationException("Character index out of bounds");
        }

        $before = mb_substr($orig_string, 0, $index);
        $after = mb_substr($orig_string, $index + 1);

        return Value::string_value($before . $replacement_char . $after);
    }

    public static function type(?Value $value): Value
    {
        if ($value === null) {
            return Value::string_value("");
        }

        return Value::string_value(
            match ($value->type) {
                Int => "int",
                Bool => "bool",
                String => "string",
                Nil => "nil",
            }
        );
    }

    public function exit(): void
    {
        match ($this->type) {
            Int => $this->int_value >= 0 && $this->int_value <= 9
                ? exit($this->int_value)
                : throw new WrongValueException(),
            default => throw new WrongTypeException()
        };
    }
    public function dprint(OutputWriter $stderr): void
    {
        $stderr->writeString(var_export($this, true));
    }

    static function parse_int(string $integer_string): int
    {
        $integer_string = strtolower($integer_string);

        return match (true) {
            str_starts_with($integer_string, "0x") => (int)hexdec($integer_string),
            str_starts_with($integer_string, "0o") => (int)octdec($integer_string),
            is_numeric($integer_string) => (int)$integer_string,
            default => throw new InvalidXMLStructureException()
        };
    }

    static function parse_string(string $raw_string): string
    {
        $i = 0;
        $result = "";
        while (mb_substr($raw_string, $i) !== "") {
            if (mb_substr($raw_string, $i, 1) === "\\") {
                $result .= mb_chr((int)mb_substr($raw_string, $i + 1, 3));
                $i += 4;
            } else {
                $result .= mb_substr($raw_string, $i, 1);
                $i++;
            }
        }

        return $result;
    }

    public static function from_xml(DOMElement $element): Argument
    {
        return match ($element->getAttribute("type")) {
            "int" => new Value(ValueType::int, int_value: Value::parse_int($element->textContent)),
            "bool" => new Value(
                ValueType::bool,
                bool_value: match ($element->textContent) {
                    "true" => true,
                    "false" => false,
                    default => throw new InvalidXMLStructureException()
                }
            ),
            "string" => new Value(ValueType::string, string_value: Value::parse_string($element->textContent)),
            "nil" => $element->textContent === "nil"
                ? new Value(ValueType::nil)
                : throw new InvalidXMLStructureException(),
            default => throw new InvalidXMLStructureException(),
        };
    }
}
