#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

extern "C" {
void increment(hls::stream<ap_axiu<32, 0, 0, 0> >& input, hls::stream<ap_axiu<32, 0, 0, 0> >& output) {

    
#pragma HLS interface ap_ctrl_none port = return

    ap_axiu<32, 0, 0, 0> v = input.read();
    v.data = v.data + 1;
    output.write(v);
}
}
