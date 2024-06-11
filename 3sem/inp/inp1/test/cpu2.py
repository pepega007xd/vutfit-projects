
# ------------------------------------------------------------------------------------------------------------
# V teto casti jsou zakladni testy, ktere jsou soucasti zadani
# ===========================================================================================================
# Zasah do teto casti neni dovolen. V pripade, ze potrebujete otestovat funkcnost lepe, zduplikujte si kod
# testu do uzivatelske casti tohoto skriptu a dopiste pozadovanou funkcionalitu
# ------------------------------------------------------------------------------------------------------------

from scripts.cpu_lib import *


@tb_test()
async def test_reset(dut):
    clk_gen = await cpu_dut_init(dut)
    # aktivovat reset, pockat 50ns, deaktivovat reset
    dut.reset.value = 1
    await Timer(50, units='ns')
    dut.reset.value = 0

    # vyhodnotit stav signalu
    assert (dut.data_en.value.binstr in ['0', '1']), "Invalid enable value"
    assert (dut.done.value.binstr == '0'), "Invalid done value"
    assert (dut.ready.value.binstr == '0'), "Invalid ready value"
    assert ((dut.data_en.value.binstr == '0') or ((dut.data_en.value.binstr == '1') and (
        dut.data_rdwr.value.binstr == '0'))), "Processor should read from the memory"
    assert (dut.in_req.value.binstr == '0'), "Can't request for input data"
    assert (dut.out_we.value.binstr == '0'), "Can't write to the output"


@tb_test()
async def test_debug_00(dut):
    instcnt, mem, lcd = await run_program(dut, 'A+@\1')
    assert mem[instcnt] == 2


@tb_test()
async def test_debug_01(dut):
    instcnt, mem, lcd = await run_program(dut, '.@9')
    assert lcd == "9"


@tb_test()
async def test_debug_02(dut):
    instcnt, mem, _ = await run_program(dut, '>+<>+-@')
    assert mem[instcnt+1] == 1


@tb_test()
async def test_debug_03(dut):
    instcnt, mem, lcd = await run_program(dut, ',@', kb_data='Q', timeout_ns=(KB_WAIT_TIME+LCD_WAIT_TIME)*5 + 100)
    assert mem[instcnt] == ord('Q')


@tb_test()
async def test_debug_04(dut):
    instcnt, mem, lcd = await run_program(dut, ',+,@', kb_data='YQ', timeout_ns=(KB_WAIT_TIME+LCD_WAIT_TIME)*5 + 100)
    assert mem[instcnt] == ord('Q')


@tb_test()
async def test_debug_05(dut):
    instcnt, mem, lcd = await run_program(dut, '[-]@\3')
    assert mem[instcnt] == 0


@tb_test()
async def test_debug_06(dut):
    instcnt, mem, lcd = await run_program(dut, '++>++<[-[>+<-]]@')
    assert mem[instcnt] == 0
    assert mem[instcnt+1] == 3


@tb_test()
async def test_debug_07(dut):
    instcnt, mem, lcd = await run_program(dut, f"""[>+>+<<-]@{chr(4)}""", timeout_ns=10_000)
    assert mem[instcnt] == 0
    assert mem[instcnt+1] == 4
    assert mem[instcnt+2] == 4


@tb_test()
async def test_debug_08(dut):
    instcnt, mem, lcd = await run_program(dut, f"""[<<+>>-]@{chr(0)}{chr(4)}{chr(4)}""", timeout_ns=10_000)
    assert mem[instcnt] == 0
    assert mem[instcnt+1] == 4
    assert mem[instcnt+2] == 4
    assert mem[instcnt+3] == 0


@tb_test()
async def test_debug_09(dut):
    instcnt, mem, lcd = await run_program(dut, f"""[->->+<<]@{chr(4)}{chr(4)}""", timeout_ns=10_000)
    assert mem[instcnt] == 0
    assert mem[instcnt+1] == 0
    assert mem[instcnt+2] == 4
    assert mem[instcnt+3] == 0


@tb_test()
async def test_debug_10(dut):
    instcnt, mem, lcd = await run_program(dut, f"""++++++++++----------+++@{chr(4)}{chr(4)}""", timeout_ns=10_000)
    assert mem[instcnt] == 7
    assert mem[instcnt+1] == 4
    assert mem[instcnt+2] == 0
    assert mem[instcnt+3] == 0


@tb_test()
async def test_debug_11(dut):
    instcnt, mem, lcd = await run_program(dut, f""">[<+>-]@{chr(0)}{chr(4)}""", timeout_ns=10_000)
    assert mem[instcnt] == 4
    assert mem[instcnt+1] == 0
    assert mem[instcnt+2] == 0
    assert mem[instcnt+3] == 0


@tb_test()
async def test_debug_12(dut):
    instcnt, mem, lcd = await run_program(dut, f""">>[<<+>>-]@{chr(0)}{chr(20)}{chr(4)}""", timeout_ns=10_000)
    assert mem[instcnt] == 4
    assert mem[instcnt+1] == 20
    assert mem[instcnt+2] == 0
    assert mem[instcnt+3] == 0


@tb_test()
async def test_debug_13(dut):
    instcnt, mem, lcd = await run_program(dut, f""">>[<<+>>-]@{chr(0)}{chr(4)}{chr(4)}""", timeout_ns=10_000)
    assert mem[instcnt] == 4
    assert mem[instcnt+1] == 4
    assert mem[instcnt+2] == 0
    assert mem[instcnt+3] == 0


@tb_test()
async def test_debug_14(dut):
    instcnt, mem, lcd = await run_program(dut, f"""[>+>+<<-]YYYYYYY>>[<<+>>-]@{chr(4)}""", timeout_ns=10_000)
    assert mem[instcnt] == 4
    assert mem[instcnt+1] == 4
    assert mem[instcnt+2] == 0
    assert mem[instcnt+3] == 0


@tb_test()
async def test_debug_15(dut):
    instcnt, mem, lcd = await run_program(dut, f"""; copy cell0 => cell2
(
  ; move cell0 => cell2 & cell3
  [>>+>+<<<-]
)
@{chr(123)}""", timeout_ns=200_000)
    assert mem[instcnt] == 123
    assert mem[instcnt+1] == 0
    assert mem[instcnt+2] == 0
    assert mem[instcnt+3] == 0


@tb_test()
async def test_debug_16(dut):
    instcnt, mem, lcd = await run_program(dut, '[>+>+>+<<<-]@\2', timeout_ns=5000)
    assert mem[instcnt] == 0
    assert mem[instcnt+1] == 2
    assert mem[instcnt+2] == 2
    assert mem[instcnt+3] == 2


@tb_test()
async def test_debug_17(dut):
    instcnt, mem, lcd = await run_program(dut, 'X[X>X+X>+X>\n+<X<<-]@\2', timeout_ns=5000)
    assert mem[instcnt] == 0
    assert mem[instcnt+1] == 2
    assert mem[instcnt+2] == 2
    assert mem[instcnt+3] == 2


@tb_test()
async def test_debug_18(dut):
    instcnt, mem, lcd = await run_program(dut, """++
[
  > +
  < -
]
>.
@""", timeout_ns=200_000)
    assert mem[instcnt] == 0
    assert mem[instcnt+1] == 2


@tb_test()
async def test_debug_19(dut):
    instcnt, mem, lcd = await run_program(dut, """+++++ +++++        bunku cislo 0 inicializovat na 10
[                  pouzit cyklus pro inicializaci nasledujicich tri bunek na 90/80/50
 > +-++++ +++++    pricist 9 k bunce cislo 1
 > +-++++ ++++     pricist 8 k bunce cislo 2
 > +++++           pricist 5 k bunce cislo 3
 <<< -             vratit se a dekrementovat hodnotu bunky cislo 0
]
>@""", timeout_ns=200_000)
    assert mem[instcnt] == 0
    assert mem[instcnt+1] == 90
    assert mem[instcnt+2] == 80
    assert mem[instcnt+3] == 50


@tb_test()
async def test_debug_20(dut):
    instcnt, mem, lcd = await run_program(dut, f"""
>--.         tisk znaku "x"
>----.       tisk znaku "l"
+++.         tisk znaku "o"
--------.    tisk znaku "g"
++.          tisk znaku "i"
+++++.       tisk znaku "n"
>--.         tisk znaku "0"
.            tisk znaku "0"
            
Tento kod lze tak jak jej vidite tj vcetne komentaru odsmimulovat
pomoci debuggeru na adrese http://www TECKA fit TECKA vutbr TECKA cz/ VLNOVKA vasicek/inp23/
Pozor za znakem zavinac nesmi byt odradkovani jinak nebude tento kod pracovat korektne protoze
ASCII hodnoty odradkovani budou pouzity jako inicializator datove casti pameti
@{chr(0)}{chr(90)}{chr(80)}{chr(50)}""", timeout_ns=200_000)
    lcd = lcd.lower()
    assert lcd == "xlogin00", "Invalid output"


@tb_test()
async def test_debug_21(dut):
    instcnt, mem, lcd = await run_program(dut, f"""+++++ +++++        bunku cislo 0 inicializovat na 10
[                  pouzit cyklus pro inicializaci nasledujicich tri bunek na 90/80/50
 > +-++++ +++++    pricist 9 k bunce cislo 1 (90=Z)
 > +-++++ ++++     pricist 8 k bunce cislo 2 (80=P)
 > +++++           pricist 5 k bunce cislo 3 (50=2)
 <<< -             vratit se a dekrementovat hodnotu bunky cislo 0
]
>--.         tisk znaku "x"
>----.       tisk znaku "l"
+++.         tisk znaku "o"
--------.    tisk znaku "g"
++.          tisk znaku "i"
+++++.       tisk znaku "n"
>--.         tisk znaku "0"
.            tisk znaku "0"
            
Tento kod lze tak jak jej vidite tj vcetne komentaru odsmimulovat
pomoci debuggeru na adrese http://www TECKA fit TECKA vutbr TECKA cz/ VLNOVKA vasicek/inp23/
Pozor za znakem zavinac nesmi byt odradkovani jinak nebude tento kod pracovat korektne protoze
ASCII hodnoty odradkovani budou pouzity jako inicializator datove casti pameti
@""", timeout_ns=2_000_000)  # increased timeout
    lcd = lcd.lower()
    assert lcd == "xlogin00", "Invalid output"


@tb_test()
async def test_debug_22(dut):
    instcnt, mem, lcd = await run_program(dut, '-Q@\2', timeout_ns=5000)
    assert mem[instcnt] == 1


@tb_test()
async def test_debug_23_2(dut):
    instcnt, mem, lcd = await run_program(dut, '+[~]+@', timeout_ns=10_000)
    assert mem[instcnt] == 2


@tb_test()
async def test_debug_23(dut):
    instcnt, mem, lcd = await run_program(dut, '+[[[[[~]~]~]~]~]+@', timeout_ns=10_000)
    assert mem[instcnt] == 2


@tb_test()
async def test_debug_24(dut):
    instcnt, mem, lcd = await run_program(dut, """[+++]>[-]@\0\5""", timeout_ns=100_000)
    assert mem[instcnt] == 0
    assert mem[instcnt+1] == 0
    assert mem[instcnt+2] == 0
    assert mem[instcnt+3] == 0


@tb_test()
async def test_debug_25(dut):
    instcnt, mem, lcd = await run_program(dut, """
[>>+>+<<<-]
>>>
[<<<+>>>-]
<<+>

[<->[>++++++++++<[->-[>+>>]>[+[-<+>]>+>>]<<<<<]>[-]++++++++[<++++++>-]>[<<+>>-]>[<<+>>-]<<]>]@\5""", timeout_ns=50_000)
    assert mem[instcnt] == 5
    assert mem[instcnt+1] == 0
    assert mem[instcnt+2] == 53
    assert mem[instcnt+3] == 0


@tb_test()
async def test_debug_26_2(dut):
    instcnt, mem, lcd = await run_program(dut, '+>[[]]+@', timeout_ns=50_000)
    assert mem[instcnt] == 1
    assert mem[instcnt+1] == 1
    assert mem[instcnt+2] == 0
    assert mem[instcnt+3] == 0


@tb_test()
async def test_debug_26(dut):
    instcnt, mem, lcd = await run_program(dut, '+>[[[[[~]~]~]~]~]+@', timeout_ns=50_000)
    assert mem[instcnt] == 1
    assert mem[instcnt+1] == 1
    assert mem[instcnt+2] == 0
    assert mem[instcnt+3] == 0


@tb_test()
async def test_debug_27(dut):
    instcnt, mem, lcd = await run_program(dut, '++++[>[[[[[~]~]~]~]~]+<-]++@', timeout_ns=50_000)
    assert mem[instcnt] == 2
    assert mem[instcnt+1] == 4
    assert mem[instcnt+2] == 0
    assert mem[instcnt+3] == 0


@tb_test()
async def test_debug_28(dut):
    instcnt, mem, lcd = await run_program(dut, '++++++++++[>+++++++>[[[[[~]~]~]~]~]>+++++<<<-]>++++@', timeout_ns=50_000)
    assert mem[instcnt] == 0
    assert mem[instcnt+1] == 74
    assert mem[instcnt+2] == 0
    assert mem[instcnt+3] == 50


@tb_test()
async def test_debug_29_2(dut):
    instcnt, mem, lcd = await run_program(dut, '+++[-]@', timeout_ns=5000)
    # assert mem[instcnt] == 1
    # assert mem[instcnt+1] == 0
    # assert mem[instcnt+2] == 0
    # assert mem[instcnt+3] == 1


@tb_test()
async def test_debug_29(dut):
    instcnt, mem, lcd = await run_program(dut, '+>[]>[[]<~][~[]~][[[]][[][[[]~]]]]>+@', timeout_ns=5000)
    assert mem[instcnt] == 1
    assert mem[instcnt+1] == 0
    assert mem[instcnt+2] == 0
    assert mem[instcnt+3] == 1


@tb_test()
async def test_break_while_while(dut):
    prog = "++++++++++[>+++++++>[~~]>+++++<<<-]>+++.+++++.++.>>-----.+++++.--.++.+.-----.<<++++++++++++++++++++++++++++++++.++.---.-----.>>+++.@"
    enableDebug(lcd=True)
    instcnt, mem, lcd = await run_program(dut, prog, timeout_ns=1_000_000)
    assert lcd == "INP-2023.proj1"


@tb_test()
async def test_break_while_while_2(dut):
    # prog = "++++++++++[>+++++++>[[~]~]>+++++<<<-]>+++.+++++.++.>>-----.+++++.--.++.+.-----.<<++++++++++++++++++++++++++++++++.++.---.-----.>>+++.@"
    prog = "++++++++++[>+++++++>[[[[[~]~]~]~]~]>+++++<<<-]>+++.+++++.++.>>-----.+++++.--.++.+.-----.<<++++++++++++++++++++++++++++++++.++.---.-----.>>+++.@"
    enableDebug(lcd=True)
    instcnt, mem, lcd = await run_program(dut, prog, timeout_ns=1_000_000)
    assert lcd == "INP-2023.proj1"


@tb_test()
async def test_hello_world(dut):
    prog = "++++++++++[>+++++++>+++[~[[[]]]]+++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.@"
    enableDebug(lcd=True)
    instcnt, mem, lcd = await run_program(dut, prog, timeout_ns=1_000_000)
    assert lcd == "Hello World!\n"


@tb_test()
async def test_init(dut):
    """Procesor initialization test"""
    instcnt, mem, _ = await run_program(dut, '@')
    assert (dut.ready.value.binstr ==
            '1'), "Invalid ready value, should be active"


@tb_test()
async def test_increment(dut):
    """Increment value of the first memory cell, i.e. *ptr++"""
    instcnt, mem, _ = await run_program(dut, '+++@')
    assert (dut.done.value.binstr == '1'), "Invalid done value, should be active"
    assert instcnt == 4
    assert mem[4] == 3


@tb_test()
async def test_decrement(dut):
    """Decrement value of the first memory cell, i.e. *ptr--"""
    instcnt, mem, _ = await run_program(dut, '---@\3')
    assert instcnt == 4
    assert mem[4] == 0


@tb_test()
async def test_move(dut):
    """Move the pointer to the next cell and increment its value"""
    instcnt, mem, _ = await run_program(dut, '>+@')
    assert instcnt == 3
    assert mem[3] == 0
    assert mem[4] == 1


@tb_test()
async def test_print(dut):
    """Print data to the output, i.e. putchar(*ptr)"""
    instcnt, mem, lcd = await run_program(dut, '...@012', timeout_ns=LCD_WAIT_TIME*5 + 1000)
    assert lcd == "000"


@tb_test()
async def test_input(dut):
    """Load data from the input, i.e. *ptr=getchar()"""
    instcnt, mem, lcd = await run_program(dut, ',,,@', kb_data='312', timeout_ns=(KB_WAIT_TIME+LCD_WAIT_TIME)*5 + 100)
    assert mem[4] == ord('2')

    instcnt, mem, lcd = await run_program(dut, ',,,@', kb_data='123', timeout_ns=(KB_WAIT_TIME+LCD_WAIT_TIME)*5 + 100)
    assert mem[4] == ord('3')


@tb_test()
async def test_while_loop(dut):
    """Simple while loop test"""
    instcnt, mem, lcd = await run_program(dut, '[.-]@\3', timeout_ns=LCD_WAIT_TIME*10)
    assert mem[5] == 0
    assert lcd == "\3\2\1"


@tb_test()
async def test_break(dut):
    """Simple break test"""
    instcnt, mem, lcd = await run_program(dut, '[-~+]@\1', timeout_ns=LCD_WAIT_TIME*10)
    assert mem[6] == 0


@tb_test()
async def test_login(dut, uid=''):
    """Executes program in login.b file"""
    file_name = '../src/login.b'
    assert os.path.isfile(file_name), "File login.b is missing"
    with open(file_name, 'rt') as f:
        prog = f.read()
    assert len(prog), "File login.b doesn't contain any program"
    # 250_000
    instcnt, mem, lcd = await run_program(dut, prog, timeout_ns=250_000)
    lcd = lcd.lower()
    assert lcd == "xbrabl04", "Invalid output"


# -------------------------------------------------------------------------------------------------------
# Uzivatelske testy
# ===========================================================================================================
# V teto casti muzete v pripade potreby vlozit jakekoliv vlastni testy.
# -------------------------------------------------------------------------------------------------------

@tb_test()  # test se spousti automaticky
async def test_example_abc(dut):
    prog = """+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.+.+.+.+.+.+.+.+.+.+.+.@"""
    instcnt, mem, lcd = await run_program(dut, prog, kb_data=f"", timeout_ns=10_000)
    assert lcd == "ABCDEFGHIJKL", "Invalid output"


@tb_test()  # test se spousti automaticky
async def test_example_hello(dut):
    prog = """.>.>.>.>.@Hello"""
    instcnt, mem, lcd = await run_program(dut, prog, kb_data=f"", timeout_ns=5_000)
    assert lcd == "Hello", "Invalid output"


@tb_test()  # test se spousti automaticky
async def test_example_text(dut):
    prog = """++++++++++[>+++++++>+++>+++++<<<-]>+++.+++++.++.>>-----.+++++.--.++.+.-----.<<++++++++++++++++++++++++++++++++.++.---.-----.>>+++.@"""
    instcnt, mem, lcd = await run_program(dut, prog, kb_data=f"", timeout_ns=40_000)
    assert lcd == "INP-2023.proj1", "Invalid output"


@tb_test()  # test se spousti automaticky
async def test_example_sum(dut):
    prog = """+++++++++++++++
secte cisla od 1 do N=15
[>+<-]>
[[>+>+<<-]>>[-<<+>>]<-]<[[>[<+>-]]<<]
>[<+>-]
<.@"""
    instcnt, mem, lcd = await run_program(dut, prog, kb_data=f"", timeout_ns=500_000)
    assert lcd == "x", "Invalid output"


@tb_test()  # test se spousti automaticky
async def test_example_hello_world(dut):
    prog = """++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.@"""
    instcnt, mem, lcd = await run_program(dut, prog, kb_data=f"", timeout_ns=40_000)
    assert lcd == "Hello World!\n", "Invalid output"


@tb_test()  # test se spousti automaticky
async def test_example_bsort(dut):
    prog = """>>,[>>,]<<[
[<<]>>>>[
<<[>+<<+>-]
>>[>+<<<<[->]>[<]>>-]
<<<[[-]>>[>+<-]>>[<<<+>>>-]]
>>[[<+>-]>>]<
]<<[>>+<<-]<<
]>>>>[.>>]@"""
    instcnt, mem, lcd = await run_program(dut, prog, kb_data=f"312\0", timeout_ns=500_000)
    assert lcd == "123", "Invalid output"


# Odkomentujte jeden z nasledujicich radku pro zarazeni do testu
# @tb_test(skip=True) #test se nespousti automaticky, spustit lze volanim TESTCASE=test_printf make
@tb_test()  # test se spousti automaticky
async def test_printf(dut):
    """Program which emulates printing of %d """

    # Priklad testu, ktery provede pozadovany program:
    # Program ocekava vstup z klavesnice, v testu je stisknuta klavesa odpovidajici hodnote 123
    prog = ',[>>+>+<<<-]>>>[<<<+>>>-]<<+>[<->[>++++++++++<[->-[>+>>]>[+[-<+>]>+>>]<<<<<]>[-]++++++++[<++++++>-]>[<<+>>-]>[<<+>>-]<<]>]<[->>++++++++[<++++++>-]]<[.[-]<]<@'
    enableDebug(lcd=True)
    instcnt, mem, lcd = await run_program(dut, prog, kb_data=chr(123), timeout_ns=1_000_000)
    assert lcd == '123', "Invalid output"


@tb_test()
async def test_pepega_01(dut):
    prog = '+++++++[-][[-]][]+@'
    enableDebug(lcd=True)
    instcnt, mem, lcd = await run_program(dut, prog, timeout_ns=1_000_000)
    assert mem[instcnt] == 1

# from now on, shamelessly stolen from https://esolangs.org/wiki/Brainfuck#Examples


@tb_test()
async def test_pepega_02(dut):
    prog = """
     1 +++++ +++               Set Cell #0 to 8
     2 [
     3     >++++               Add 4 to Cell #1; this will always set Cell #1 to 4
     4     [                   as the cell will be cleared by the loop
     5         >++             Add 4*2 to Cell #2
     6         >+++            Add 4*3 to Cell #3
     7         >+++            Add 4*3 to Cell #4
     8         >+              Add 4 to Cell #5
     9         <<<<-           Decrement the loop counter in Cell #1
    10     ]                   Loop till Cell #1 is zero
    11     >+                  Add 1 to Cell #2
    12     >+                  Add 1 to Cell #3
    13     >-                  Subtract 1 from Cell #4
    14     >>+                 Add 1 to Cell #6
    15     [<]                 Move back to the first zero cell you find; this will
    16                         be Cell #1 which was cleared by the previous loop
    17     <-                  Decrement the loop Counter in Cell #0
    18 ]                       Loop till Cell #0 is zero
    19 
    20 The result of this is:
    21 Cell No :   0   1   2   3   4   5   6
    22 Contents:   0   0  72 104  88  32   8
    23 Pointer :   ^
    24 
    25 >>.                     Cell #2 has value 72 which is 'H'
    26 >---.                   Subtract 3 from Cell #3 to get 101 which is 'e'
    27 +++++ ++..+++.          Likewise for 'llo' from Cell #3
    28 >>.                     Cell #5 is 32 for the space
    29 <-.                     Subtract 1 from Cell #4 for 87 to give a 'W'
    30 <.                      Cell #3 was set to 'o' from the end of 'Hello'
    31 +++.----- -.----- ---.  Cell #3 for 'rl' and 'd'
    32 >>+.                    Add 1 to Cell #5 gives us an exclamation point
    33 >++.                    And finally a newline from Cell #6
    @"""
    enableDebug(lcd=True)
    instcnt, mem, lcd = await run_program(dut, prog, timeout_ns=1_000_000)
    assert lcd == "Hello World!\n"


@tb_test()
async def test_pepega_03(dut):
    prog = """
        ++++++++[>++++++++<-]>++++++++.>++++++++[>++++++++++++<-]>+++++.+++++++..+++.>++++++++[>+++++<-]>++++.------------.<<<<+++++++++++++++.>>.+++.------.--------.>>+.
    @"""
    enableDebug(lcd=True)
    instcnt, mem, lcd = await run_program(dut, prog, timeout_ns=1_000_000)
    assert lcd == "Hello, World!"


@tb_test()
async def test_pepega_04(dut):
    prog = """
        >++++++++[-<+++++++++>]<.>>+>-[+]++>++>+++[>[->+++<<+++>]<<]>-----.>->
        +++..+++.>-.<<+[>[+>+]>>]<--------------.>>.+++.------.--------.>+.>+.
    @"""
    enableDebug(lcd=True)
    instcnt, mem, lcd = await run_program(dut, prog, timeout_ns=1_000_000)
    assert lcd == "Hello World!\n"


@tb_test()
async def test_pepega_05(dut):
    prog = """
        Code:   Pseudo code:
        >>      Move the pointer to cell2
        [-]     Set cell2 to 0 
        <<      Move the pointer back to cell0
        [       While cell0 is not 0
          -       Subtract 1 from cell0
          >>      Move the pointer to cell2
          +       Add 1 to cell2
          <<      Move the pointer back to cell0
        ]       End while
    @\x42"""
    enableDebug(lcd=True)
    instcnt, mem, lcd = await run_program(dut, prog, kb_data=chr(123), timeout_ns=1_000_000)
    assert mem[instcnt:instcnt+4] == [0, 0, 0x42, 0]


@tb_test()
async def test_pepega_06(dut):
    prog = """
,[.,]
    @"""
    enableDebug(lcd=True)
    instcnt, mem, lcd = await run_program(dut, prog, kb_data="hello there\000", timeout_ns=1_000_000)
    assert lcd == "hello there"


@tb_test()
async def test_pepega_07(dut):
    prog = """
        >+[
            <[
                [>>+<<-]>[<<+<[->>+[<]]>>>[>]<<-]<<<
            ]>>[<<+>>-]<[>+<-]>[>>]<,
        ]<<<[<+<]>[>.>]
    @"""
    enableDebug(lcd=True)
    instcnt, mem, lcd = await run_program(dut, prog, kb_data="3142\000", timeout_ns=10_000_000)
    assert lcd == "1234"


@tb_test()
async def test_pepega_08(dut):
    prog = """
        >>+>>>>>,[>+>>,]>+[--[+<<<-]<[<+>-]<[<[->[<<<+>>>>+<-]<<[>>+>[->]<<[<]
        <-]>]>>>+<[[-]<[>+<-]<]>[[>>>]+<<<-<[<<[<<<]>>+>[>>>]<-]<<[<<<]>[>>[>>
        >]<+<<[<<<]>-]]+<<<]+[->>>]>>]>>[.>>>]
    @"""
    enableDebug(lcd=True)
    # i wanted to let it sort "vitejte-v-inp-2023" but it's slow as hell
    instcnt, mem, lcd = await run_program(dut, prog, kb_data="2143\000", timeout_ns=10_000_000)
    assert lcd == "1234"


@tb_test()
async def test_pepega_09(dut):
    prog = """
    +[+++~<weirdness> ~~[[]][[[][[[[~]][]][]][[]~]~[[]]~~]] </weirdness>[]]+
    @"""
    enableDebug(lcd=True)
    instcnt, mem, lcd = await run_program(dut, prog, kb_data="", timeout_ns=1_000_000)
    assert mem[instcnt:instcnt+4] == [5, 0, 0, 0]
    assert lcd == ""


@tb_test()
async def test_pepega_10(dut):
    prog = """
    kentus blentus (divili byste se; ale timhle jsem realne jednou odhalil bug v kodu xdddd)
    @"""
    enableDebug(lcd=True)
    instcnt, mem, lcd = await run_program(dut, prog, kb_data="", timeout_ns=1_000_000)
    assert mem[instcnt:instcnt+4] == [0, 0, 0, 0]
    assert lcd == ""


@tb_test()
async def test_pepega_11(dut):
    prog = """
[~][][~]@\000
 @"""
    enableDebug(lcd=True)
    instcnt, mem, lcd = await run_program(dut, prog, kb_data="", timeout_ns=1_000_000)
    assert mem[instcnt:instcnt+4] == [0, ord('\n'), ord(' '), ord('@')]
    assert lcd == ""


@tb_test()
async def test_pepega_12(dut):
    prog = """+[~[[][]]]@\000@a@"""
    enableDebug(lcd=True)
    instcnt, mem, lcd = await run_program(dut, prog, kb_data="", timeout_ns=1_000_000)
    assert mem[instcnt:instcnt+4] == [1, ord('@'), ord('a'), ord('@')]
    assert lcd == ""

if __name__ == "__main__":
    # call the main function
    print("Run this simulation by calling make.")
    exit()
