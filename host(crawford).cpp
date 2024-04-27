#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>

#include "cmdlineparser.h"



const int MAXHIT = 64;
const int NLABEL = 4;


int main(int argc, char** argv) {

    // Command Line Parser
    sda::utils::CmdLineParser parser;

    // Switches
    //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
    parser.addSwitch("--xclbin_file", "-x", "input binary file string", "");
    parser.addSwitch("--device_id", "-d", "device index", "0");
    parser.parse(argc, argv);

    // Read settings
    std::string binaryFile = parser.value("xclbin_file");
    int device_index = stoi(parser.value("device_id"));

    if (argc < 3) {
        parser.printHelp();
        return EXIT_FAILURE;
    }
    std::cout << "Open the device" << device_index << std::endl;
    auto device = xrt::device(device_index);
    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device.load_xclbin(binaryFile);

    size_t data_size = 1024 * 1024;
    size_t vector_size_bytes = sizeof(int) * data_size;

    // Reducing the data size for emulation mode
    char* xcl_mode = getenv("XCL_EMULATION_MODE");
    if (xcl_mode != nullptr) {
        data_size = 1024;
    }



    std::string data = "/apotto/home1/aliens/bianchinir/lavoro/dataset_tools/2472509/file1.txt"; 
    std::string coord = "/apotto/home1/aliens/bianchinir/lavoro/dataset_tools/2472509/coord.txt";
    std::vector<std::vector<int>> hitList;
    std::vector<int> labelList;


    auto algoritmo = xrt::kernel(device, uuid, "algoritmo");

    auto buffer_input1 = xrt::bo(device, vector_size_bytes, increment.group_id(0));
    auto buffer_input2 = xrt::bo(device, vector_size_bytes, increment.group_id(0));
    auto buffer_output = xrt::bo(device, vector_size_bytes, increment.group_id(0));

    auto x = buffer_input1.map<int*>();
    auto y = buffer_input2.map<int*>();
    auto rings_arrayK = buffer_output.map<int*>();



    //CARICAMENTO COORDINATE

    std::vector<std::vector<int>> coordinate;
    int j=0;
    std::ifstream loadcoord(coord);
    if (!loadcoord.is_open()) {
        std::cerr << "Failed to open file: " << coord << std::endl;
        continue;
    }
    std::string line1
    while(std::getline(loadcoord, line1)){
        
        std::string number1;
        std::stringstream ss(line1);
        std::vector<std::string> linedata1;
        
        while(ss >> number1){
            	linedata1.push_back(number1);
            }

        
        coordinate[j][0]=std::stoi(linedata1[0]);
        coordinate[j][1]=std::stof(linedata1[1]);
        coordinate[j][2]=std::stof(linedata1[2]);
        
        j=j+1;
    }   
    
    std::ifstream loadcoord.close();

    //FINE CARICAMENTO COORDINATE

    
    std::ifstream loaddata(data);
    if (!loaddata.is_open()) {
       std::cerr << "Failed to open file: " << data << std::endl;
         continue;
    }



    std::string line2;
    std::vector<int> rings_array(4, 0);

    while (std::getline(loadfile, line2)) {

        	std::string number2;
            std::stringstream ss(line2);
            std::vector<std::string> linedata2;

            while(ss >> number2){
            	linedata2.push_back(number2);
            }

            if (linedata2.size() < 10) continue; // Skip lines that are too short

            int type = std::stoi(linedata2[3]);

            if (type == 20) {
                // Process RICH ring data
                rings_array[0] = std::stof(linedata2[5]);
                rings_array[1] = std::stof(linedata2[6]);
                rings_array[2] = std::stof(linedata2[7]);
            }


            if (type == 23) {
                // Process summary line with from RICH reco
                std::string id_ev = linedata2[0];
                
                rings_array[3] = std::stoi(linedata2[4]);
                
                int hit = std::stoi(linedata2[9]);
                
                if (hit > MAXHIT) hit = MAXHIT;

               // std::vector<int> x(hit,0);
               // std::vector<int> y(hit,0);
                std::vector<int> list(hit,0)
                
                
                for (int f = 10; f < (10 + hit); ++f){
                    list[f-10]= std::stoi(linedata2[f]);
                    x[f-10]=0;
                    y[f-10]=0;
                }
                
                for(int b=0;b<hit,++b){
                    for(int k=0;k<coordinate.size(),++k){                    
                        if(coordinate[k][0]==list[b]){
                            x[b]=coordinate[k][1];
                            y[b]=coordinate[k][2];
                        }
                    }
                }


                // Copy input data to device global memory
                std::cout << "Copying data1..." << std::endl;
                buffer_input1.sync(XCL_BO_SYNC_BO_TO_DEVICE);
                std::cout << "Copying data2..." << std::endl;
                buffer_input2.sync(XCL_BO_SYNC_BO_TO_DEVICE);

                // Launch the Kernel
                std::cout << "Launching Kernel..." << std::endl;
                auto algoritmo_run = xrt::run(algoritmo);
                algoritmo_run.set_arg(0, buffer_input1);
                algortimo_run.set_arg(1, buffer_input2);
                algortimo_run.set_arg(2, buffer_output);
                algortimo_run.set_arg(3, hit);
                algortimo_run.start();
    

                algortimo_run.wait();

                // Copy Result from Device Global Memory to Host Local Memory
                std::cout << "Getting Results..." << std::endl;
                buffer_output.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
                
                std::cout << "RAGGIO: DA FILE -> " << rings_array[0] << " vs " << rings_arrayK[0] << " <- DA ALGO " << std::endl;
                std::cout << "X: DA FILE -> " << rings_array[0] << " vs " << rings_arrayK[0] << " <- DA ALGO " << std::endl;
                std::cout << "Y: DA FILE -> " << rings_array[0] << " vs " << rings_arrayK[0] << " <- DA ALGO " << std::endl;
                
    

            
            }
        }
    std::ifstream loaddata.close();

    return 0;
}
