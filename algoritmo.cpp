#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>
#include <vector>

extern "C" {
void increment(int x, int y, int* output, int hit ) {

    
#pragma HLS interface ap_ctrl_none port = return

    /*ap_axiu<32, 0, 0, 0> list = input.read();
    st::vector<int> x(hit,0), y(hit,0);
    //    QUALCHE MODO PER ANDARE DA list a x[i] e y[i] (coordinate x e y di ogni PMT indicizzate con i che arriva fino a hit)

    int x_m = 0, y_m = 0;
    for(int i = 0; i < hit; ++i){
        x_m = x_m + x[i];
        y_m = y_m + y[i];
    }

    x_m = x_m/hit;
    y_m = y_m/hit;

    for(int i = 0; i < hit; ++i){
        x[i] = x[i] - x_m;
        y[i] = y[i] - y_m;
    }*/
    
    std::vector<int> xy(hit,0), x2(hit,0), y2(hit,0), x3(hit,0), y3(hit,0), x2y(hit,0), xy2(hit,0);
    int XY = 0, X2 = 0, Y2 = 0, X3 = 0, Y3 = 0, X2Y = 0, XY2 = 0;
    
    for(int i = 0; i < hit; ++1){
        xy[i] = x[i] * y[i];
        x2[i] = x[i] * x[i];
        y2[i] = y[i] * y[i];
        x3[i] = x[i] * x[i] * x[i];
        y3[i] = y[i] * y[i] * y[i];
        x2y[i] = x[i] * x[i] * y[i];
        xy2[i] = x[i] * y[i] * y[i];
    }

    for(int i = 0; i < hit; ++i){
        XY = XY + xy[i];
        X2 = X2 + x2[i];
        Y2 = Y2 + y2[i];
        X3 = X3 + x3[i];
        Y3 = Y3 + y3[i];
        X2Y = X2Y + x2y[i];
        XY2 = XY2 + xy2[i];
    }

    int x0, y0, r;
    //ULTIME COSE PER CALCOLARE CARATTERISTICHE CERCHIO

    //TUTTO QUESTO E' PER UN CERCHIO, AGGIUNGERE ANCHE ALTRE CASISTICHE CIOE' ZERO CERCHI DUE E TRE

    std::vector<int> ring(3,0)

    output[0] = x0;
    output[1] = y0;
    output[2] = r;

    output.write(ring);
}
}
