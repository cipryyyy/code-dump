// LED pendulum
// Tang Nano 9K

module top 
(
    input btn1,             // Reset button
    input clk,              // Internal 27MHz clock
    output reg[5:0] led     // LED register
);

reg[31:0] delay_counter;                            // Counter for the delay for 0.5s delay
reg direction;                                      // Update direction
reg[2:0] led_index;                                 // Index of the lit LED   
localparam integer CLOCK_FREQUENCY = 27_000_000;    // sys_clk frequency
localparam integer ms = CLOCK_FREQUENCY / 1000;     // ms equivalent in clock cycles

// Delay management
always @(posedge clk or negedge btn1) begin
    // Reset logic
    if (!btn1) begin
        delay_counter <= 32'd0;     // Reset delay counter
        direction <= 1'b0;          // Reset direction
    end

    // Delay elapsed
    else if (delay_counter == (250 * ms - 1)) begin
        delay_counter <= 32'd0;                 // Reset delay counter

        // Value updated at the end of the clock, so check the next LED and update direction
        if ((led_index - 1) == 3'd0)
            direction <= 1'b1;
        else if ((led_index + 1) == 3'd5)
            direction <= 1'b0;
    end

    // Continue delay
    else begin
        delay_counter <= delay_counter + 32'd1;
    end
end

// LED management
always @(posedge clk or negedge btn1) begin
    // Reset logic
    if (!btn1) begin
        led <= 6'b011111;       // Reset LED
        led_index <= 3'd5;      // Reset index
    end

    // Delay elapsed
    else if (delay_counter == (500*ms - 1)) begin
        if (direction == 0) begin
            led_index <= led_index - 1'b1;      // Update index
            led[5:0] <= {led[0], led[5:1]};     // Rotate register
        end else begin
            led_index <= led_index + 1'b1;
            led[5:0] <= {led[4:0], led[5]};
        end
    end
end

endmodule
