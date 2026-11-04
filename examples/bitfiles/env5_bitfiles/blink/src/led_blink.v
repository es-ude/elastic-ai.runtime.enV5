// Blinks the LEDs of the elastic-node FPGA.
//
// The blink frequency is set by the BLINK_HZ parameter. CLK_HZ must match the
// frequency of the clock that drives the `clk` port.
//
// Change the defaults below, or override them when instantiating the module:
//
//   led_blink #(.CLK_HZ(100_000_000), .BLINK_HZ(2)) blink (.clk(clk), .leds(leds));

`timescale 1ns / 1ps

module led_blink #(
    parameter integer CLK_HZ   = 100_000_000,  // frequency of `clk` in Hz
    parameter integer BLINK_HZ = 1,            // LED blink frequency in Hz
    parameter integer NUM_LEDS = 4             // number of LEDs to drive
) (
    input  wire                clk,
    output wire [NUM_LEDS-1:0] leds
);

  // The LED toggles twice per blink period (on and off).
  localparam integer HALF_PERIOD_CYCLES = CLK_HZ / (2 * BLINK_HZ);
  localparam integer COUNTER_WIDTH = $clog2(HALF_PERIOD_CYCLES + 1);

  reg [COUNTER_WIDTH-1:0] counter = 0;
  reg                     led_state = 1'b0;

  always @(posedge clk) begin
    if (counter >= HALF_PERIOD_CYCLES - 1) begin
      counter   <= 0;
      led_state <= ~led_state;
    end else begin
      counter <= counter + 1'b1;
    end
  end

  assign leds = {NUM_LEDS{led_state}};

endmodule
