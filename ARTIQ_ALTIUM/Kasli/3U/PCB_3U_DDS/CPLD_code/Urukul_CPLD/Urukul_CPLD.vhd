----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    19:21:25 09/07/2017 
-- Design Name: 
-- Module Name:    Urukul_CPLD - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity Urukul_CPLD is
    Port ( ATT_CLK	: out  STD_LOGIC;
           ATT_LE	: out  STD_LOGIC;
           ATT_RSTN	: out  STD_LOGIC;
           ATT_S_IN	: out  STD_LOGIC;
           ATT_S_OUT	: in  STD_LOGIC;
           DDS_CSN : out  STD_LOGIC_VECTOR (3 downto 0);
           DDS_IO_RESET	: out  STD_LOGIC;
           DDS_PLL_LOCK : in  STD_LOGIC_VECTOR (3 downto 0);
           DDS_RESET	: out  STD_LOGIC;
           DDS_SCKI : out  STD_LOGIC_VECTOR (3 downto 0);
           DDS_SDI : out  STD_LOGIC_VECTOR (3 downto 0);
           DDS_SDO : in  STD_LOGIC_VECTOR (3 downto 0);
           DDS_SYNC_CLK0 : in  STD_LOGIC;
           DDS_SYNC_CLKSEL : out  STD_LOGIC;
           DDS_SYNC_CLK_OUTEN	: out  STD_LOGIC;
           DDS_SYNC_OUTEN: out  STD_LOGIC;
           DDS_UPDATE : out  STD_LOGIC_VECTOR (3 downto 0);
           IFC_MODE_SEL : in  STD_LOGIC_VECTOR (3 downto 0);
           IO_A : inout  STD_LOGIC_VECTOR (7 downto 1);
           IO_B8_SCKI	: inout  STD_LOGIC;
           IO_B : inout  STD_LOGIC_VECTOR (15 downto 9);
           LED : out  STD_LOGIC_VECTOR (7 downto 0);
           PROFILE_P : out  STD_LOGIC_VECTOR (2 downto 0);
           RFSW_CTRL : out  STD_LOGIC_VECTOR (3 downto 0);
           REn_DE : out  STD_LOGIC_VECTOR (15 downto 0);
			  CLK_IN_SEL : out  STD_LOGIC;
           IO_A0_SCKI : inout  STD_LOGIC;
			  CLK_DIV : out  STD_LOGIC;
           SYNC_SMP_ERR : in  STD_LOGIC_VECTOR (3 downto 0);
           TP : out  STD_LOGIC_VECTOR (4 downto 0));
end Urukul_CPLD;

architecture Behavioral of Urukul_CPLD is

signal reset,ioreg0_LE, ioreg1_LE, shiftreg_scki, shiftreg_sdi : std_logic;
signal shiftreg, ioreg0,ioreg1 : std_logic_vector(15 downto 0);
signal chipsel : std_logic_vector(2 downto 0);
signal cs : std_logic_vector(7 downto 0);

begin

reset <= '0';

--  serial control shift register
process (shiftreg_scki, reset)
begin
   if reset ='1' then 
      shiftreg <= (others => '0'); 
   elsif shiftreg_scki'event and shiftreg_scki='1' then  
         for i in 0 to 14 loop
            shiftreg(i+1) <= shiftreg(i);
         end loop;
         shiftreg(0) <= shiftreg_sdi;
   end if;
end process;

--SR0 latch
process (ioreg0_LE, reset)
begin
   if reset ='1' then 
      ioreg0 <= (others => '0'); 
   elsif ioreg0_LE'event and ioreg0_LE='1' then  
			ioreg0 <= shiftreg;
   end if;
end process;
--SR1 latch
process (ioreg1_LE, reset)
begin
   if reset ='1' then 
      ioreg1 <= (others => '0'); 
   elsif ioreg1_LE'event and ioreg1_LE='1' then  
			ioreg1 <= shiftreg;
   end if;
end process;

	
	
		chipsel <= IO_A(5 downto 3);
		cs(1) 	<= '0' when chipsel="001" else '1';
		cs(2) 	<= '0' when chipsel="010" else '1';
		cs(3)  	<= '0' when chipsel="011" else '1';
		cs(4) 	<= '0' when chipsel="100" else '1';
		cs(5) 	<= '0' when chipsel="100" else '1';
		cs(6) 	<= '0' when chipsel="100" else '1';
		cs(7) 	<= '0' when chipsel="100" else '1';

process (IFC_MODE_SEL)
begin
	case IFC_MODE_SEL(1 downto 0) is
	when "00" =>
		-- this is 9910/9912 mode with single EEM control
		PROFILE_P <= ioreg0(15 downto 13);
		RFSW_CTRL <= ioreg0( 3 downto 0);
		LED  <= ioreg1(15 downto 8);

		REn_DE (0) <= '0'; -- CLK, input
		REn_DE (1) <= '0'; -- MOSI, input
		REn_DE (2) <= '1'; -- MISO, output
		REn_DE (3) <= '0'; -- CS0, input
		REn_DE (4) <= '0'; -- CS1, input
		REn_DE (5) <= '0'; -- CS2, input
		REn_DE (6) <= '0'; -- IO_UPDATE, input
		REn_DE (7) <= '0'; -- DDS_RESET, input
		REn_DE (8) <= '0'; -- not used, input
		REn_DE (9) <= '0'; -- not used, input
		REn_DE (10) <= '0'; -- not used, input
		REn_DE (11) <= '0'; -- not used, input
		REn_DE (12) <= '0'; -- not used, input
		REn_DE (13) <= '0'; -- not used, input
		REn_DE (14) <= '0'; -- not used, input
		REn_DE (15) <= '0'; -- not used, input

		DDS_UPDATE <= IO_A(6) & IO_A(6) & IO_A(6) & IO_A(6);

		DDS_SYNC_OUTEN <= '0';
		DDS_SYNC_CLKSEL  <= '0';
		DDS_SYNC_CLK_OUTEN	 <= '0';
		ATT_CLK <= IO_A0_SCKI;

		ATT_RSTN	<= '1';
		ATT_S_IN <= IO_A(1);


		DDS_IO_RESET <= IO_A(7);

		DDS_RESET <= IO_A(7);
		DDS_SCKI <= IO_A0_SCKI & IO_A0_SCKI & IO_A0_SCKI & IO_A0_SCKI;
		DDS_SDI <= IO_A(1) &  IO_A(1) &  IO_A(1) &  IO_A(1);
		
		ioreg0_LE 	<= cs(1);
		ioreg1_LE 	<= cs(2);
		ATT_LE  		<= cs(3);
		DDS_CSN(0) 	<= cs(4);
		DDS_CSN(1) 	<= cs(5);
		DDS_CSN(2) 	<= cs(6);
		DDS_CSN(3) 	<= cs(7);
		shiftreg_scki <= IO_A0_SCKI;
		shiftreg_sdi <= IO_A(1);

when "01" =>
		-- this is 9910/9912 mode with dual EEM control
		PROFILE_P <= ioreg0(15 downto 13);
		RFSW_CTRL <= IO_B( 15 downto 12);
		LED  <= ioreg1(15 downto 8);

		REn_DE (0) <= '0'; -- CLK, input
		REn_DE (1) <= '0'; -- MOSI, input
		REn_DE (2) <= '1'; -- MISO, output
		REn_DE (3) <= '0'; -- CS0, input
		REn_DE (4) <= '0'; -- CS1, input
		REn_DE (5) <= '0'; -- CS2, input
		REn_DE (6) <= '0'; -- IO_UPDATE, input
		REn_DE (7) <= '0'; -- DDS_RESET, input
		REn_DE (8) <= '0'; -- not used, input
		REn_DE (9) <= '0'; -- not used, input
		REn_DE (10) <= '0'; -- not used, input
		REn_DE (11) <= '0'; -- IO_Update, output by FAN LVDS chip
		REn_DE (12) <= '0'; -- RF SW0, input
		REn_DE (13) <= '0'; -- RF SW1, input
		REn_DE (14) <= '0'; -- RF SW2, input
		REn_DE (15) <= '0'; -- RF SW3, input

		DDS_UPDATE <= IO_A(6) & IO_A(6) & IO_A(6) & IO_A(6);

		DDS_SYNC_OUTEN <= '0';
		DDS_SYNC_CLKSEL  <= '0';
		DDS_SYNC_CLK_OUTEN	 <= '1';
		ATT_CLK <= IO_A0_SCKI;

		ATT_RSTN	<= '1';
		ATT_S_IN <= IO_A(1);


		DDS_IO_RESET <= IO_A(7);

		DDS_RESET <= IO_A(7);
		DDS_SCKI <= IO_A0_SCKI & IO_A0_SCKI & IO_A0_SCKI & IO_A0_SCKI;
		DDS_SDI <= IO_A(1) &  IO_A(1) &  IO_A(1) &  IO_A(1);
		
		ioreg0_LE 	<= cs(1);
		ioreg1_LE 	<= cs(2);
		ATT_LE  		<= cs(3);
		DDS_CSN(0) 	<= cs(4);
		DDS_CSN(1) 	<= cs(5);
		DDS_CSN(2) 	<= cs(6);
		DDS_CSN(3) 	<= cs(7);
		shiftreg_scki <= IO_A0_SCKI;
		shiftreg_sdi <= IO_A(1);

		
	when others =>
			-- this is 9910 mode with QSPI
		PROFILE_P <= ioreg0(15 downto 13);
		LED  <= ioreg0(7 downto 0);

		REn_DE (0) <= '0'; -- CLK, input
		REn_DE (1) <= '0'; -- MOSI0, input
		REn_DE (2) <= '0'; -- MOSI1, input
		REn_DE (3) <= '0'; -- MOSI2, input
		REn_DE (4) <= '0'; -- MOSI3, input
		REn_DE (5) <= '0'; -- CS, input
		REn_DE (6) <= '0'; -- IO_UPDATE, input
		REn_DE (7) <= '0'; -- DDS_SYNC, output by FAN chip
		REn_DE (8) <= '0'; -- SR CLK, input
		REn_DE (9) <= '0'; -- SR MOSI, input
		REn_DE (10) <= '0'; -- SR_MISO, output
		REn_DE (11) <= '0'; -- SR_CS, input
		REn_DE (12) <= '0'; -- not used, input
		REn_DE (13) <= '0'; -- not used, input
		REn_DE (14) <= '0'; -- not used, input
		REn_DE (15) <= '0'; -- not used, input

		DDS_UPDATE <= IO_A(6) & IO_A(6) & IO_A(6) & IO_A(6);

		DDS_SYNC_OUTEN <= '1';
		DDS_SYNC_CLKSEL  <= '1';
		DDS_SYNC_CLK_OUTEN	 <= '1';

		ATT_CLK <= IO_B8_SCKI;

		ATT_RSTN	<= '1';
		ATT_S_IN <= IO_B(9);
		IO_B(10) <= ATT_S_OUT;

		ioreg0_LE 	<= IO_A(5);
		ATT_LE  		<= not IO_A(5);
		DDS_CSN(0) 	<= IO_A(5);
		DDS_CSN(1) 	<= IO_A(5);
		DDS_CSN(2) 	<= IO_A(5);
		DDS_CSN(3) 	<= IO_A(5);
	

		DDS_RESET <= IO_A(7);
		DDS_SCKI <= IO_A0_SCKI & IO_A0_SCKI & IO_A0_SCKI & IO_A0_SCKI;
		DDS_SDI <= IO_A(4) & IO_A(3) & IO_A(2) & IO_A(1);
		shiftreg_scki <= IO_B8_SCKI;
		shiftreg_sdi <= IO_B(9);
		
	end case;
	end process;
	
		


	

		CLK_IN_SEL <= IFC_MODE_SEL(2);
		CLK_DIV <= '1' when IFC_MODE_SEL(3)='1' else 'Z';

end Behavioral;

