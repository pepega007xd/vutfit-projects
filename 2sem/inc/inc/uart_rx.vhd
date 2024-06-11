-- uart_rx.vhd: UART controller - receiving (RX) side
-- Author(s): Tomas Brablec (xbrabl04)

library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.ALL;
use ieee.std_logic_unsigned.all;



-- Entity declaration (DO NOT ALTER THIS PART!)
entity UART_RX is
    port(
        CLK      : in std_logic;
        RST      : in std_logic;
        DIN      : in std_logic;
        DOUT     : out std_logic_vector(7 downto 0);
        DOUT_VLD : out std_logic
    );
end entity;



-- Architecture implementation (INSERT YOUR IMPLEMENTATION HERE)
architecture behavioral of UART_RX is
    signal shift_en: std_logic;
    signal vld: std_logic;

    signal ctr_8: std_logic;
    signal ctr_8_en: std_logic;
    signal ctr_8_reg: unsigned(3 downto 0) := (others => '0'); 
    signal ctr_8_rst: std_logic;

    signal ctr_15: std_logic;
    signal ctr_16: std_logic;
    signal ctr_16_en: std_logic;
    signal ctr_16_reg: unsigned(3 downto 0) := (others => '0'); 

    signal shift_reg: std_logic_vector(7 downto 0) := (others => '0');
begin

    -- Instance of RX FSM
    fsm: entity work.UART_RX_FSM
    port map (
        CLK => CLK,
        RST => RST,
        DIN => DIN,

        CTR_8 => ctr_8,
        CTR_8_EN => ctr_8_en,
        CTR_8_RST => ctr_8_rst,

        CTR_16 => ctr_16,
        CTR_16_EN => ctr_16_en,

        SHIFT_EN => shift_en,
        VLD => vld
    );

    ctr_8 <= '1' when ctr_8_reg = 8 else '0';
    ctr_15 <= '1' when ctr_16_reg = 14 else '0';
    ctr_16 <= '1' when ctr_16_reg = 15 else '0';
    DOUT_VLD <= '1' when vld = '1' and ctr_16 = '1' else '0';

    shift_register: process(CLK, shift_en, ctr_16)
    begin
        if rising_edge(CLK) and shift_en = '1' and ctr_15 = '1' then
            shift_reg <= DIN & shift_reg(7 downto 1);
        end if;
        DOUT <= shift_reg;
    end process;

    counter_8: process(CLK)
    begin
        if rising_edge(CLK) then
            if ctr_8_rst = '1' then
                ctr_8_reg <= (others => '0');
            elsif ctr_8_en = '1' or ctr_16 = '1' then
                ctr_8_reg <= ctr_8_reg + 1;
            end if;
        end if;
    end process;

    counter_16: process(CLK)
    begin
        if rising_edge(CLK) and ctr_16_en = '1' then
            ctr_16_reg <= ctr_16_reg + 1;
        end if;
    end process;

end architecture;
