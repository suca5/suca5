#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>
#include <vector>
#include <cmath>

extern "C" {
void increment(float* x, float* y, float* output, int hit ) {

    
#pragma HLS INTERFACE m_axi port = x bundle = gmem0
#pragma HLS INTERFACE m_axi port = y bundle = gmem0
#pragma HLS INTERFACE m_axi port = output bundle = gmem0

    /*ap_axiu<32, 0, 0, 0> list = input.read();
    st::vector<int> x(hit,0), y(hit,0);
    //    QUALCHE MODO PER ANDARE DA list a x[i] e y[i] (coordinate x e y di ogni PMT indicizzate con i che arriva fino a hit)
*/
    float x_m = 0, y_m = 0;
    for(int i = 0; i < hit; ++i){
        #pragma HLS UNROLL
        x_m = x_m + x[i];
        y_m = y_m + y[i];
    }

    x_m = x_m/hit;
    y_m = y_m/hit;

    /*for(int i = 0; i < hit; ++i){
        x[i] = x[i] - x_m;
        y[i] = y[i] - y_m;
    }*/
    
    std::vector<float> xy(hit,0), x2(hit,0), y2(hit,0), x3(hit,0), y3(hit,0), x2y(hit,0), xy2(hit,0);
    float XY = 0, X2 = 0, Y2 = 0, X3 = 0, Y3 = 0, X2Y = 0, XY2 = 0;
    
    for(int i = 0; i < hit; ++1){
        #pragma HLS UNROLL
        xy[i] = x[i] * y[i];
        x2[i] = x[i] * x[i];
        y2[i] = y[i] * y[i];
        x3[i] = x[i] * x[i] * x[i];
        y3[i] = y[i] * y[i] * y[i];
        x2y[i] = x[i] * x[i] * y[i];
        xy2[i] = x[i] * y[i] * y[i];
    }

    for(int i = 0; i < hit; ++i){
        #pragma HLS UNROLL
        XY = XY + xy[i];
        X2 = X2 + x2[i];
        Y2 = Y2 + y2[i];
        X3 = X3 + x3[i];
        Y3 = Y3 + y3[i];
        X2Y = X2Y + x2y[i];
        XY2 = XY2 + xy2[i];
    }

    float x0, y0, r, A, B, C, D, E, F;
    //ULTIME COSE PER CALCOLARE CARATTERISTICHE CERCHIO

    #pragma HLS DATAFLOW
    A=(X2 - (x_m * x_m * hit));
    B=(XY - (x_m * y_m * hit));
    C=(X3 + XY2 - (x_m * (X2 + Y2)));
    D=(XY2- (x_m * y_m * hit));
    E=(Y2 - (y_m * y_m * hit));
    F=(X2Y + Y3 - (y_m * (X2 + Y2)));
    x0=0.5 * ((F - (E * (C / B))) / (D - (E * (A / B))));
    y0=0.5 * ((C - (A * (F / D))) / (B - (A * (E / D))));
    r=sqrt((x0 * x0) + (y0 * y0) - ((2 * x0 * x_m) + (2 * y0 * y_m) - (X2 / hit) - (Y2 / hit)));

    
    //TUTTO QUESTO E' PER UN CERCHIO, AGGIUNGERE ANCHE ALTRE CASISTICHE CIOE' ZERO CERCHI DUE E TRE

    //std::vector<float> ring(3,0)

    output[0] = x0;
    output[1] = y0;
    output[2] = r;

    //output.write(ring);
}
}
