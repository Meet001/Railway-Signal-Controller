----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    15:20:26 01/23/2018 
-- Design Name: 
-- Module Name:    decrypter - Behavioral 
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
use IEEE.STD_LOGIC_UNSIGNED.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity decrypter is
    Port ( clock : in  STD_LOGIC;
           K : in  STD_LOGIC_VECTOR (31 downto 0);
           C : in  STD_LOGIC_VECTOR (31 downto 0);
           P : out  STD_LOGIC_VECTOR (31 downto 0);
           reset : in  STD_LOGIC;
	   done:out STD_LOGIC;
           enable : in  STD_LOGIC);
end decrypter;

architecture Behavioral of decrypter is
signal T:std_logic_vector (3 downto 0);
signal Curr_P:std_logic_vector (31 downto 0); -- works as a temporary value holder of P
signal x: integer range 0 to 64 := 0;	-- Used for calculation of N0 and intial assignment of P and T 
													--	and first step of incrementing T
signal N0:integer range 0 to 32 := 0;	-- used for storing number of 0's in K
signal iterator:integer range 0 to 32 :=0;	-- Used as loop control for encoding of P
begin
	
--	x <= 0;
   process(clock, reset, enable)
	begin

		if (reset = '1') then
			
			done <= '0';
			x <= 0;
			N0 <= 0;
			iterator <= 0;
			
		elsif (clock'event and clock = '1' and enable = '1') then
		
			if(x <=31) then  --  for calculation of N0
				done <= '0';
				if(K(x) = '0') then
					N0 <= N0 + 1;
				end if;
				x <= x + 1;
				
			elsif(x = 32) then  -- intial assignment of T and P
				done <= '0';
				T(3) <= K(3) XOR K(7) XOR K(11) XOR K(15) XOR K(19) XOR K(23) XOR K(27) XOR K(31);
				T(2) <= K(2) XOR K(6) XOR K(10) XOR K(14) XOR K(18) XOR K(22) XOR K(26) XOR K(30);
				T(1) <= K(1) XOR K(5) XOR K(9) XOR K(13) XOR K(17) XOR K(21) XOR K(25) XOR K(29);
				T(0) <= K(0) XOR K(4) XOR K(8) XOR K(12) XOR K(16) XOR K(20) XOR K(24) XOR K(28);
				Curr_P <= C;
				x <= x+1;
				
			elsif(x = 33) then 
				done <= '0';
				T <= T + "1111";
				x <= x+1;
				
			elsif(iterator <= N0-1 and x = 34) then -- decoding of C to produce P
				done <= '0';
				Curr_P <= Curr_P XOR (T & T & T & T & T & T & T & T);
				T <= T + "1111";
				iterator <= iterator + 1;
				
			elsif(iterator = N0 and x = 34) then -- assigning decoded value of C to P
				P <= Curr_P ;
				done <= '1';
				
			end if;
		end if;	
			
	 end process;

end Behavioral;
