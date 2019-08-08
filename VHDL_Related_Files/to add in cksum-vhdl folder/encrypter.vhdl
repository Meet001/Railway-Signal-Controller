----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    14:22:45 01/23/2018 
-- Design Name: 
-- Module Name:    encrypter - Behavioral 
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
use IEEE.NUMERIC_STD.ALL;
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity encrypter is
    Port ( clock : in  STD_LOGIC;
           K : in  STD_LOGIC_VECTOR (31 downto 0);
           P : in  STD_LOGIC_VECTOR (31 downto 0);
           C : out  STD_LOGIC_VECTOR (31 downto 0);
           reset : in  STD_LOGIC;
			  done:out STD_LOGIC;
           enable : in  STD_LOGIC);
end encrypter;

architecture Behavioral of encrypter is
signal T:std_logic_vector (3 downto 0);
signal Curr_C:std_logic_vector (31 downto 0); -- works as a temporary value holder of C
signal x: integer range 0 to 33 := 0;	-- Used for calculation of N1 and intial assignment of C and T
signal N1:integer range 0 to 32 := 0;	-- used for storing number of 1's in K
signal iterator:integer range 0 to 32 :=0;	-- Used as loop control for encoding of P
begin
	
--	x <= 0;
   process(clock, reset, enable)
	begin
	 
	 
		if (reset = '1') then
			
			done <= '0';
			x <= 0;
			N1 <= 0;
			iterator <= 0;
				
		elsif (clock'event and clock = '1' and enable = '1') then
		
			if(x <=31) then --  for calculation of N1
				done <= '0';
				if(K(x) = '1') then
					N1 <= N1 + 1;
				end if;
				x <= x + 1;
				
			elsif(x = 32) then -- intial assignment of T and C
				T(3) <= K(3) XOR K(7) XOR K(11) XOR K(15) XOR K(19) XOR K(23) XOR K(27) XOR K(31);
				T(2) <= K(2) XOR K(6) XOR K(10) XOR K(14) XOR K(18) XOR K(22) XOR K(26) XOR K(30);
				T(1) <= K(1) XOR K(5) XOR K(9) XOR K(13) XOR K(17) XOR K(21) XOR K(25) XOR K(29);
				T(0) <= K(0) XOR K(4) XOR K(8) XOR K(12) XOR K(16) XOR K(20) XOR K(24) XOR K(28);
				Curr_C <= P;
				x <= x+1;
				done <= '0';
				
			elsif(iterator <= N1-1 and x = 33) then -- encoding of P to produce C
				done <= '0';
				Curr_C <= Curr_C XOR (T & T & T & T & T & T & T & T);
				T <= T + "0001";
				iterator <= iterator + 1;
				
			elsif(iterator = N1 and x = 33) then -- assigning encoded value of P to C
				C <= Curr_C;
				done <= '1';					
				
			end if;
		end if;	
		
			
	 end process;

end Behavioral;

