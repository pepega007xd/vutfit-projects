<?php

namespace IPP\Student;

use IPP\Student\Instruction;
use DOMElement;
use IPP\Core\AbstractInterpreter;
use IPP\Core\Interface\OutputWriter;
use IPP\Core\ReturnCode;

// Contains the internal state of the interpreter,
// public interface is used to execute instructions
class Interpreter extends AbstractInterpreter
{
    /** @var array<Instruction> $instructions */
    private array $instructions = [];
    private int $instruction_pointer = 0;
    /** @var array<string, int> $label_locations */
    private array $label_locations = [];

    /** @var array<Frame> $local_frames */
    private array $local_frames = [];
    private Frame $global_frame;
    private ?Frame $temp_frame = null;

    /** @var array<Value> $value_stack */
    private array $value_stack = [];

    /** @var array<int> $call_stack */
    private array $call_stack = [];

    private InstructionHandler $instruction_handler;

    // sets up the initial state of interpreter, parses and validates input program, and runs it
    public function execute(): int
    {
        // `new` cannot be an implicit value of attribute
        $this->global_frame = new Frame();

        $program = $this->source->getDOMDocument();
        if (!$program->schemaValidateSource(SCHEMA)) {
            throw new InvalidXMLStructureException();
        }

        $program = $program->firstChild ?? throw new InvalidXMLStructureException();

        // parse instructions
        foreach ($program->childNodes as $instr) {
            if ($instr instanceof DOMElement) {
                array_push($this->instructions, new Instruction($instr));
            }
        }

        // sort instructions by order
        usort($this->instructions, fn ($lhs, $rhs) => $lhs->order - $rhs->order);

        // detect duplicate instructions
        $instruction_orders = array_map(fn ($instr) => $instr->order, $this->instructions);
        if ($instruction_orders !== array_unique($instruction_orders)) {
            throw new InvalidXMLStructureException("Duplicate instruction orders");
        }

        // fill label locations
        foreach (array_values($this->instructions) as $idx => $instr) {
            if ($instr->opcode === "LABEL") {
                $label_arg = $instr->arguments[0];
                if ($label_arg instanceof Label) {
                    if (array_key_exists($label_arg->name, $this->label_locations)) {
                        throw new SemanticErrorException("Duplicate label");
                    }

                    $this->label_locations[$label_arg->name] = $idx;
                }
            }
        }

        // pass I/O handlers into instruction handler
        $this->instruction_handler = new InstructionHandler(
            $this,
            $this->input,
            $this->stdout,
            $this->stderr
        );

        // run program
        return $this->run_program();
    }

    public function create_frame(): void
    {
        $this->temp_frame = new Frame;
    }

    public function push_frame(): void
    {
        array_push(
            $this->local_frames,
            $this->temp_frame ?? throw new FrameAccessException("Temporary frame does not exist")
        );
        $this->temp_frame = null;
    }

    public function pop_frame(): void
    {
        $this->temp_frame = array_pop($this->local_frames)
            ?? throw new FrameAccessException("Local frame does not exist");
    }

    function get_frame(Variable $var): Frame
    {
        return match ($var->frame) {
            FrameType::Global => $this->global_frame,

            FrameType::Local => empty($this->local_frames)
                ? throw new FrameAccessException("Local frame does not exist")
                : end($this->local_frames),

            FrameType::Temporary => $this->temp_frame
                ?? throw new FrameAccessException("Temporary frame does not exist")
        };
    }

    public function define_var(Variable $var): void
    {
        $this->get_frame($var)->define_var($var);
    }

    // returns value stored in variable, throws exception if variable is uninitialized
    public function get_value(Variable $var): Value
    {
        return $this->get_frame($var)->get_value($var)
            ?? throw new MissingValueException("Variable has no value");
    }

    // returns value stored in variable, returns null if variable is uninitialized
    public function get_value_opt(Variable $var): ?Value
    {
        return $this->get_frame($var)->get_value($var);
    }

    public function set_value(Variable $var, Value $value): void
    {
        $this->get_frame($var)->set_value($var, $value);
    }

    public function stack_push(Value $value): void
    {
        array_push($this->value_stack, $value);
    }

    public function stack_pop(Variable $var): void
    {
        if (empty($this->value_stack)) {
            throw new MissingValueException("Stack is empty");
        }

        $this->set_value($var, array_pop($this->value_stack));
    }

    public function jump(Label $label): void
    {
        if (array_key_exists($label->name, $this->label_locations)) {
            $this->instruction_pointer = $this->label_locations[$label->name];
        } else {
            throw new SemanticErrorException("Jump to nonexistent label");
        }
    }

    public function call(Label $label): void
    {
        // instruction_pointer already has the address of the *following*
        // instruction to the one being executed, push that to the call stack
        array_push($this->call_stack, $this->instruction_pointer);
        $this->jump($label);
    }

    public function return(): void
    {
        if (empty($this->call_stack)) {
            throw new MissingValueException("Call stack is empty");
        } else {
            $origin = array_pop($this->call_stack);
            $this->instruction_pointer = $origin;
        }
    }

    public function break(OutputWriter $stderr): void
    {
        $stderr->writeString(var_export($this, true));
    }

    // executes the instructions of the program until either the end is reached,
    // or an EXIT instruction is executed
    public function run_program(): int
    {
        while (true) {
            // check for end of program
            if (!array_key_exists($this->instruction_pointer, $this->instructions)) {
                return ReturnCode::OK;
            }

            $instr = $this->instructions[$this->instruction_pointer];

            // during the execution of an instruction,
            // instruction_pointer must refer to the next instruction
            $this->instruction_pointer++;

            $instr_handler = $this->instruction_handler->get_handler($instr->opcode);
            $arg_types = InstructionHandler::get_arg_types($instr->opcode);

            // replace variables with their current values, if the instruction handler
            // expects a value
            $processed_arguments = [];
            foreach (array_map(null, $arg_types, $instr->arguments) as [$arg_type, $arg]) {
                // TYPE instruction can work with uninitialized variables,
                // replace argument with null (get_value_opt) instead of throwing exception (get_value)
                $replaced_arg = match ($arg_type) {
                    ArgumentType::Value => $arg instanceof Variable
                        ? ($instr->opcode === "TYPE"
                            ? $this->get_value_opt($arg)
                            : $this->get_value($arg))
                        : $arg,
                    default => $arg
                };

                array_push($processed_arguments, $replaced_arg);
            }

            // run instruction
            ($instr_handler)(...$processed_arguments);
        }
    }
}

const SCHEMA = <<<END
<?xml version="1.0"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema"
elementFormDefault="qualified">

<xs:simpleType name="ordertype">
  <xs:restriction base="xs:integer">
    <xs:minInclusive value="1"/>
  </xs:restriction>
</xs:simpleType>

<xs:complexType name="argtype">
  <xs:simpleContent>
    <xs:extension base="xs:string">
      <xs:attribute name="type" type="xs:string" use="required"/>
    </xs:extension>
  </xs:simpleContent>
</xs:complexType>

<xs:element name="program">
  <xs:complexType>
    <xs:sequence minOccurs="0" maxOccurs="unbounded">
      <xs:element name="instruction">
        <xs:complexType>
          <xs:all>
            <xs:element name="arg1" type="argtype" minOccurs="0" maxOccurs="1" />
            <xs:element name="arg2" type="argtype" minOccurs="0" maxOccurs="1" />
            <xs:element name="arg3" type="argtype" minOccurs="0" maxOccurs="1" />
          </xs:all>
          <xs:attribute name="opcode" type="xs:string" use="required"/>
          <xs:attribute name="order" type="ordertype" use="required"/>
        </xs:complexType>
      </xs:element> 
    </xs:sequence>
    <xs:attribute name="language" type="xs:string" use="required" fixed="IPPcode24"/>
    <xs:attribute name="name" type="xs:string"/>
    <xs:attribute name="description" type="xs:string"/>
  </xs:complexType>
</xs:element> 

</xs:schema>
END;
