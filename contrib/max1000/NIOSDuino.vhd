library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all; 

      
ENTITY NIOSDuino IS

PORT (
  CLK                        : IN    STD_LOGIC;
  Reset                      : in    std_logic                     := 'X';             
  i2c_sda_in                 : in    std_logic                     := 'X';             
  i2c_scl_in                 : in    std_logic                     := 'X';             
  i2c_sda_oe                 : out   std_logic;                                        
  i2c_scl_oe                 : out   std_logic;                                        
  pio_export                 : inout std_logic_vector(15 downto 0) := (others => 'X'); 
  sdram_addr                 : out   std_logic_vector(11 downto 0);                    
  sdram_ba                   : out   std_logic_vector(1 downto 0);                     
  sdram_cas_n                : out   std_logic;                                        
  sdram_cke                  : out   std_logic;                                        
  sdram_cs_n                 : out   std_logic;                                        
  sdram_dq                   : inout std_logic_vector(15 downto 0) := (others => 'X'); 
  sdram_dqm                  : out   std_logic_vector(1 downto 0);                     
  sdram_ras_n                : out   std_logic;                                        
  sdram_we_n                 : out   std_logic;     
  sdram_clk_clk              : out   std_logic;                                       --      sdram_clk.clk  
  spi_MISO                   : in    std_logic                     := 'X';             
  spi_MOSI                   : out   std_logic;                                        
  spi_SCLK                   : out   std_logic;                                        
  spi_SS_n                   : out   std_logic;
  spi_flash_MISO             : in    std_logic                     := '0';             --      spi_flash.MISO
  spi_flash_MOSI             : out   std_logic;                                        --               .MOSI
  spi_flash_SCLK             : out   std_logic;                                        --               .SCLK
  spi_flash_SS_n             : out   std_logic;                                        --               .SS_n
  uart_rxd                   : in    std_logic                     := 'X';             
  uart_txd                   : out   std_logic                                         

);
END NIOSDuino;

ARCHITECTURE BEHAVIORAL OF NIOSDuino IS

  COMPONENT nios_duino_max1000 IS
  PORT (
      clk_in_clk            : in    std_logic                     := '0';             --         clk_in.clk
      i2c_sda_in            : in    std_logic                     := '0';             --      i2c_0_ext.sda_in
      i2c_scl_in            : in    std_logic                     := '0';             --               .scl_in
      i2c_sda_oe            : out   std_logic;                                        --               .sda_oe
      i2c_scl_oe            : out   std_logic;                                        --               .scl_oe
      pio_export            : inout std_logic_vector(15 downto 0) := (others => '0'); --        pio_led.export
      reset_reset_n         : in    std_logic                     := '0';             --          reset.reset_n
      sdram_addr            : out   std_logic_vector(11 downto 0);                    --          sdram.addr
      sdram_ba              : out   std_logic_vector(1 downto 0);                     --               .ba
      sdram_cas_n           : out   std_logic;                                        --               .cas_n
      sdram_cke             : out   std_logic;                                        --               .cke
      sdram_cs_n            : out   std_logic;                                        --               .cs_n
      sdram_dq              : inout std_logic_vector(15 downto 0) := (others => '0'); --               .dq
      sdram_dqm             : out   std_logic_vector(1 downto 0);                     --               .dqm
      sdram_ras_n           : out   std_logic;                                        --               .ras_n
      sdram_we_n            : out   std_logic;                                        --               .we_n
      sdram_clk_clk         : out   std_logic;                                        --      sdram_clk.clk
      spi_MISO              : in    std_logic                     := '0';             --      spi_0_ext.MISO
      spi_MOSI              : out   std_logic;                                        --               .MOSI
      spi_SCLK              : out   std_logic;                                        --               .SCLK
      spi_SS_n              : out   std_logic;                                        --               .SS_n
      spi_flash_MISO        : in    std_logic                     := '0';             --      spi_flash.MISO
      spi_flash_MOSI        : out   std_logic;                                        --               .MOSI
      spi_flash_SCLK        : out   std_logic;                                        --               .SCLK
      spi_flash_SS_n        : out   std_logic;                                        --               .SS_n
      uart_rxd              : in    std_logic                     := '0';             --           uart.rxd
      uart_txd              : out   std_logic                                         --               .txd             

  );
  END COMPONENT;
  
BEGIN

  nios_duino1 : nios_duino_max1000  PORT MAP (
    clk_in_clk              => CLK,
    reset_reset_n           => Reset,
    i2c_sda_in              => i2c_sda_in,
    i2c_scl_in              => i2c_scl_in,
    i2c_sda_oe              => i2c_sda_oe,
    i2c_scl_oe              => i2c_scl_oe,
    pio_export              => pio_export,
    sdram_addr              => sdram_addr,
    sdram_ba                => sdram_ba,
    sdram_cas_n             => sdram_cas_n,
    sdram_cke               => sdram_cke,
    sdram_cs_n              => sdram_cs_n,
    sdram_dq                => sdram_dq,
    sdram_dqm               => sdram_dqm,
    sdram_ras_n             => sdram_ras_n,
    sdram_we_n              => sdram_we_n,
    sdram_clk_clk           => sdram_clk_clk,
    spi_MISO                => spi_MISO,
    spi_MOSI                => spi_MOSI,
    spi_SCLK                => spi_SCLK,
    spi_SS_n                => spi_SS_n,
    spi_flash_MISO          => spi_flash_MISO,
    spi_flash_MOSI          => spi_flash_MOSI,
    spi_flash_SCLK          => spi_flash_SCLK,
    spi_flash_SS_n          => spi_flash_SS_n,
    uart_rxd                => uart_rxd,
    uart_txd                => uart_txd
  );
  
END BEHAVIORAL;
