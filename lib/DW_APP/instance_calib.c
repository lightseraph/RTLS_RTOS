
#include "compiler.h"
#include "deca_device_api.h"
#include "instance.h"
#include "port.h"

#define DWT_PRF_64M_RFDLY (514.462f)
#define DWT_PRF_16M_RFDLY (514.288f)

// -------------------------------------------------------------------------------------------------------------------

// The table below specifies the default TX spectrum configuration parameters... this has been tuned for DW EVK hardware units
// the table is set for smart power - see below in the instance_config function how this is used when not using smart power
const tx_struct txSpectrumConfig[8] =
    {
        // Channel 0 ----- this is just a place holder so the next array element is channel 1
        {
            0x0, // 0
            {
                0x0, // 0
                0x0  // 0
            }},
        // Channel 1
        {
            0xc9, // PG_DELAY
            {
                0x15355575, // 16M prf power
                0x07274767  // 64M prf power
            }

        },
        // Channel 2
        {
            0xc2, // PG_DELAY
            {
                0x15355575, // 16M prf power
                0x07274767  // 64M prf power
            }},
        // Channel 3
        {
            0xc5, // PG_DELAY
            {
                0x0f2f4f6f, // 16M prf power
                0x2b4b6b8b  // 64M prf power
            }},
        // Channel 4
        {
            0x95, // PG_DELAY
            {
                0x1f1f3f5f, // 16M prf power
                0x3a5a7a9a  // 64M prf power
            }},
        // Channel 5
        {
            0x34, // PG_DELAY
            {
                0x0E082848, // 16M prf power
                0x25456585  // 64M prf power
            }},
        // Channel 6 ----- this is just a place holder so the next array element is channel 7
        {
            0x0, // 0
            {
                0x0, // 0
                0x0  // 0
            }},
        // Channel 7
        {
            0x93, // PG_DELAY
            {
                0x32527292, // 16M prf power
                0x5171B1d1  // 64M prf power
            }}};

// these are default antenna delays for EVB1000, these can be used if there is no calibration data in the DW1000,
// or instead of the calibration data
const uint16_t rfDelays[2] = {
    (uint16_t)((DWT_PRF_16M_RFDLY / 2.0) * 1e-9 / DWT_TIME_UNITS), // PRF 16
    (uint16_t)((DWT_PRF_64M_RFDLY / 2.0) * 1e-9 / DWT_TIME_UNITS)};

int instance_starttxtest(int framePeriod, uint8_t channel)
{
    // define some test data for the tx buffer
    uint8_t msg[127] = "The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the l";

    // NOTE: SPI frequency must be < 3MHz
    port_set_dw_ic_spi_slowrate(); // max SPI before PLLs configured is ~4M

    // the value here 0x1000 gives a period of 32.82 �s
    // this is setting 0x1000 as frame period (125MHz clock cycles) (time from Tx en - to next - Tx en)
    dwt_configcontinuousframemode((uint32_t)framePeriod, channel);

    dwt_writetxdata(127, (uint8_t *)msg, 0);
    dwt_writetxfctrl(127, 0, 0);

    // to start the first frame - set TXSTRT
    dwt_starttx(DWT_START_TX_IMMEDIATE);

    // measure the power
    // Spectrum Analyser set:
    // FREQ to be channel default e.g. 3.9936 GHz for channel 2
    // SPAN to 1GHz
    // SWEEP TIME 1s
    // RBW and VBW 1MHz
    // measure channel power

    return DWT_SUCCESS;
}

// -------------------------------------------------------------------------------------------------------------------
// extern instance_data_t instance_data[NUM_INST] ;

/* ==========================================================

Notes:

Previously code handled multiple instances in a single console application

Now have changed it to do a single instance only. With minimal code changes...(i.e. kept [instance] index but it is always 0.

Windows application should call instance_init() once and then in the "main loop" call instance_run().

*/
