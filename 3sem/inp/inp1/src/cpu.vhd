-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2023 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Tomas Brablec <xbrabl04@stud.fit.vutbr.cz>
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic;                      -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'

   -- stavove signaly
   READY    : out std_logic;                      -- hodnota 1 znamena, ze byl procesor inicializovan a zacina vykonavat program
   DONE     : out std_logic                       -- hodnota 1 znamena, ze procesor ukoncil vykonavani programu (narazil na instrukci halt)
 );
end cpu;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------

 -- pri tvorbe kodu reflektujte rady ze cviceni INP, zejmena mejte na pameti, ze 
 --   - nelze z vice procesu ovladat stejny signal,
 --   - je vhodne mit jeden proces pro popis jedne hardwarove komponenty, protoze pak
 --      - u synchronnich komponent obsahuje sensitivity list pouze CLK a RESET a 
 --      - u kombinacnich komponent obsahuje sensitivity list vsechny ctene signaly. 

architecture behavioral of cpu is
  -- general-purpose types
  subtype ADDR_TYPE is std_logic_vector (12 downto 0);
  subtype DATA_TYPE is std_logic_vector (7 downto 0);
  -- FSM signals
  type FSM_STATE_TYPE is (INIT, FIND_DATA, FETCH, DECODE, EXECUTE, WRITEBACK, PRINT_DATA, GET_DATA, IF_ZERO, FIND_END_1,FIND_END_2,FIND_END_3, IF_NONZERO, FIND_START_1, FIND_START_2, FIND_START_3, WAIT_CC);
  signal FSM_STATE: FSM_STATE_TYPE;
  -- INSTR_DECODE signal
  type INSTR_TYPE is (PLUS, MINUS, PRINT, GET, MOVE_LEFT, MOVE_RIGHT, LEFT_BRAC, RIGHT_BRAC, BREAK, AT_SIGN, NOP);
  signal INSTR: INSTR_TYPE;
  -- PTR register signals
  signal PTR_INC: std_logic;
  signal PTR_DEC: std_logic;
  signal PTR_OUT: ADDR_TYPE;
  -- PC register signals
  signal PC_INC: std_logic;
  signal PC_DEC: std_logic;
  signal PC_OUT: ADDR_TYPE;
  -- MUX_ADDR signals
  type SEL_ADDR_TYPE is (SEL_ADDR_PTR, SEL_ADDR_PC);
  signal SEL_ADDR: SEL_ADDR_TYPE;
  -- MUX_WDATA signals
  type SEL_WDATA_TYPE is (SEL_WDATA_IN, SEL_WDATA_RDATA_INC, SEL_WDATA_RDATA_DEC);
  signal SEL_WDATA: SEL_WDATA_TYPE;
  -- CNT signals
  signal CNT_INC: std_logic;
  signal CNT_DEC: std_logic;
  signal CNT_OUT: ADDR_TYPE; -- maximum number of [ can be the number of addresses
begin

  -- state machine driving the CPU
  FSM: process(CLK, RESET)
  begin
    if RESET = '1' then
      FSM_STATE <= INIT;
      PTR_INC <= '0';
      PTR_DEC <= '0';
      PC_INC <= '0';
      PC_DEC <= '0';
      SEL_ADDR <= SEL_ADDR_PTR;
      CNT_INC <= '0';
      CNT_DEC <= '0';
      DATA_RDWR <= '0';
      DATA_EN <= '0';
      IN_REQ <= '0';
      OUT_WE <= '0';
      READY <= '0';
      DONE <= '0';
    elsif rising_edge(CLK) and EN = '1' then
      case FSM_STATE is

        when INIT =>
          DATA_EN <= '1';
          FSM_STATE <= FIND_DATA;

        when FIND_DATA =>
          if INSTR = AT_SIGN then
            PTR_INC <= '0';
            PTR_DEC <= '1'; -- mitigating an off-by-one overshoot of first data cell
            SEL_ADDR <= SEL_ADDR_PC;
            FSM_STATE <= FETCH;
            READY <= '1';
          else
            PTR_INC <= '1';
          end if;

        when FETCH =>
          CNT_DEC <= '0';
          OUT_WE <= '0';
          PTR_INC <= '0';
          PTR_DEC <= '0';
          SEL_ADDR <= SEL_ADDR_PC;
          DATA_RDWR <= '0';
          PC_INC <= '0';
          PC_DEC <= '0';
          FSM_STATE <= DECODE;

        when DECODE =>
          -- wait for memory to give data
          SEL_ADDR <= SEL_ADDR_PTR;
          FSM_STATE <= EXECUTE;

        when PRINT_DATA =>
          if OUT_BUSY = '0' then
            OUT_DATA <= DATA_RDATA;
            OUT_WE <= '1';
            PC_INC <= '1';
            FSM_STATE <= FETCH;
          end if;

        when GET_DATA =>
          if IN_VLD = '1' then
            PC_INC <= '1';
            DATA_RDWR <= '0';
            IN_REQ <= '0';
            FSM_STATE <= FETCH;
          end if;

        when WRITEBACK =>
          DATA_RDWR <= '1';
          PC_INC <= '1';
          FSM_STATE <= FETCH;

        when IF_ZERO =>
          PC_INC <= '1';
          if DATA_RDATA = "00000000" then
            CNT_INC <= '1';
            FSM_STATE <= FIND_END_1;
          else
            FSM_STATE <= FETCH;
          end if;

        when FIND_END_1 =>
          PC_INC <= '0';
          CNT_INC <= '0';
          CNT_DEC <= '0';
          FSM_STATE <= FIND_END_2;

        when FIND_END_2 =>
          FSM_STATE <= FIND_END_3;
        -- wait for memory to update DATA_RDATA

        when FIND_END_3 =>
          if CNT_OUT = 0 then
            FSM_STATE <= FETCH;
          elsif INSTR = LEFT_BRAC then
            CNT_INC <= '1';
            PC_INC <= '1';
            FSM_STATE <= FIND_END_1;
          elsif INSTR = RIGHT_BRAC then
            CNT_DEC <= '1';
            PC_INC <= '1';
            FSM_STATE <= FIND_END_1;
          else
            PC_INC <= '1';
            FSM_STATE <= FIND_END_1;
          end if;

        when IF_NONZERO =>
          if DATA_RDATA /= "00000000" then
            PC_DEC <= '1';
            CNT_INC <= '1';
            FSM_STATE <= FIND_START_1;
          else
            PC_INC <= '1';
            FSM_STATE <= FETCH;
          end if;

        when FIND_START_1 =>
          PC_DEC <= '0';
          CNT_INC <= '0';
          CNT_DEC <= '0';
          FSM_STATE <= FIND_START_2;

        when FIND_START_2 =>
          FSM_STATE <= FIND_START_3;
        -- wait for memory to update DATA_RDATA

        when FIND_START_3 =>
          if CNT_OUT = 0 then
            PC_INC <= '1'; -- we must skip two instructions
            FSM_STATE <= WAIT_CC;
          elsif INSTR = LEFT_BRAC then
            CNT_DEC <= '1';
            PC_DEC <= '1';
            FSM_STATE <= FIND_START_1;
          elsif INSTR = RIGHT_BRAC then
            CNT_INC <= '1';
            PC_DEC <= '1';
            FSM_STATE <= FIND_START_1;
          else
            PC_DEC <= '1';
            FSM_STATE <= FIND_START_1;
          end if;

        when WAIT_CC => 
          FSM_STATE <= FETCH;


        when EXECUTE =>
          case INSTR is

            when PLUS =>
              SEL_WDATA <= SEL_WDATA_RDATA_INC;
              FSM_STATE <= WRITEBACK;

            when MINUS =>
              SEL_WDATA <= SEL_WDATA_RDATA_DEC;
              FSM_STATE <= WRITEBACK;

            when PRINT =>
                FSM_STATE <= PRINT_DATA;

            when GET =>
              IN_REQ <= '1';
              DATA_RDWR <= '1';
              SEL_WDATA <= SEL_WDATA_IN;
              FSM_STATE <= GET_DATA;

            when MOVE_LEFT =>
              PTR_DEC <= '1';
              PC_INC <= '1';
              FSM_STATE <= FETCH;

            when MOVE_RIGHT =>
              PTR_INC <= '1';
              PC_INC <= '1';
              FSM_STATE <= FETCH;

            when LEFT_BRAC =>
              SEL_ADDR <= SEL_ADDR_PC;
              FSM_STATE <= IF_ZERO;

            when RIGHT_BRAC =>
              SEL_ADDR <= SEL_ADDR_PC;
              FSM_STATE <= IF_NONZERO;

            when BREAK =>
              SEL_ADDR <= SEL_ADDR_PC;
              CNT_INC <= '1';
              PC_INC <= '1';
              FSM_STATE <= FIND_END_1;

            when AT_SIGN =>
              DONE <= '1';

            when NOP =>
              PC_INC <= '1';
              FSM_STATE <= FETCH;
          end case;

      end case;
    end if;
  end process FSM;

  -- mux for selecting DATA_ADDR (either PTR register, or PC register)
  MUX_ADDR: process(PTR_OUT, PC_OUT, SEL_ADDR)
  begin
    case SEL_ADDR is
      when SEL_ADDR_PTR =>
        DATA_ADDR <= PTR_OUT;
      when SEL_ADDR_PC =>
        DATA_ADDR <= PC_OUT;
    end case;
  end process MUX_ADDR;

  -- mux for selecting DATA_WDATA (RDATA_INC, RDATA_DEC, or IN_DATA)
  MUX_WDATA: process(IN_DATA, DATA_RDATA, SEL_WDATA)
  begin
    case SEL_WDATA is
      when SEL_WDATA_IN =>
        DATA_WDATA <= IN_DATA;
      when SEL_WDATA_RDATA_INC =>
        DATA_WDATA <= DATA_RDATA + 1;
      when SEL_WDATA_RDATA_DEC =>
        DATA_WDATA <= DATA_RDATA - 1;
    end case;
  end process MUX_WDATA;

  -- PTR register 
  PTR: process(CLK, RESET)
  begin
    if RESET = '1' then
      PTR_OUT <= (others => '0');
    elsif rising_edge(CLK) then
      if PTR_INC = '1' then
        PTR_OUT <= PTR_OUT + 1;
      elsif PTR_DEC = '1' then
        PTR_OUT <= PTR_OUT - 1;
      end if;
    end if;
  end process PTR;

  -- PC register 
  PC: process(CLK, RESET)
  begin
    if RESET = '1' then
      PC_OUT <= (others => '0');
    elsif rising_edge(CLK) then
      if PC_INC = '1' then
        PC_OUT <= PC_OUT + 1;
      elsif PC_DEC = '1' then
        PC_OUT <= PC_OUT - 1;
      end if;
    end if;
  end process PC;

  -- CNT register 
  CNT: process(CLK, RESET)
  begin
    if RESET = '1' then
      CNT_OUT <= (others => '0');
    elsif rising_edge(CLK) then
      if CNT_INC = '1' then
        CNT_OUT <= CNT_OUT + 1;
      elsif CNT_DEC = '1' then
        CNT_OUT <= CNT_OUT - 1;
      end if;
    end if;
  end process CNT;

  -- instruction decoder
  INSTR_DECODE: process(DATA_RDATA)
  begin
    case DATA_RDATA is
	    when "00111110" => -- >
        INSTR <= MOVE_RIGHT;
	    when "00111100" => -- <
        INSTR <= MOVE_LEFT;
	    when "00101011" => -- +
        INSTR <= PLUS;
	    when "00101101" => -- -
        INSTR <= MINUS;
	    when "00101110" => -- .
        INSTR <= PRINT;
	    when "00101100" => -- ,
        INSTR <= GET;
	    when "01011011" => -- [
        INSTR <= LEFT_BRAC;
	    when "01011101" => -- ]
        INSTR <= RIGHT_BRAC;
      when "01111110" => -- ~
        INSTR <= BREAK;
      when "01000000" => -- @
        INSTR <= AT_SIGN;
      when others =>
        INSTR <= NOP;
    end case;
  end process INSTR_DECODE;

end behavioral;
