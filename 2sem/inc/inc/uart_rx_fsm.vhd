-- uart_rx_fsm.vhd: UART controller - finite state machine controlling RX side
-- Author(s): Tomas Brablec (xbrabl04)

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity UART_RX_FSM is
    port(
        CLK: in std_logic;
        RST: in std_logic;
        DIN: in std_logic;

        CTR_8 : in std_logic;
        CTR_8_EN: out std_logic;
        CTR_8_RST: out std_logic;

        CTR_16 : in std_logic;
        CTR_16_EN: out std_logic;

        SHIFT_EN: out std_logic;
        VLD: out std_logic
    );
end entity;

architecture behavioral of UART_RX_FSM is
    type state_type is (IDLE, START, READ, STOP);
    signal curr_state: state_type := IDLE;
    signal next_state: state_type := IDLE;
begin
    clock_cycle: process(CLK)
    begin
        if rising_edge(CLK) then
            if RST = '1' then
                curr_state <= IDLE;
            else
                curr_state <= next_state;
            end if;
        end if;
    end process;

    state_change: process(DIN, CTR_8, CTR_16)
    begin
        next_state <= curr_state;
        case curr_state is
            when IDLE => 
                if DIN = '0' then
                    next_state <= START;
                end if;

            when START =>
                if CTR_8 = '1' then
                    next_state <= READ;
                end if;

            when READ => 
                if CTR_8 = '1' then
                    next_state <= STOP;
                end if;

            when STOP => 
                if CTR_16 = '1' then
                    next_state <= IDLE;
                end if;
        end case;
    end process;

    moore_outputs: process(curr_state)
    begin
        SHIFT_EN <= '0';
        CTR_16_EN <= '0';
        VLD <= '0';
        case curr_state is
            when READ =>
                SHIFT_EN <= '1';
                CTR_16_EN <= '1';
            when STOP =>
                CTR_16_EN <= '1';
                VLD <= '1';
            when others =>
        end case;
    end process;

    mealy_outputs: process(next_state, CTR_8)
    begin
        CTR_8_EN <= '0';
        CTR_8_RST <= '0';
        case next_state is
            when START =>
                CTR_8_EN <= '1';
            when READ => 
                if (CTR_8 = '1') then
                    CTR_8_RST <= '1';
                end if;
            when STOP => 
                CTR_8_RST <= '1';
            when IDLE => 
                CTR_8_RST <= '1';
        end case;
    end process;
end architecture;
