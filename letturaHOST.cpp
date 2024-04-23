#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "cmdlineparser.h"
#include <cstring>

// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

const int MAXHIT = 64;
const int NLABEL = 4;

void read_data_formatRECO2(std::vector<std::string> listfiles, std::vector<std::vector<int>>& hitList, std::vector<int>& labelList) {
    int totevt = 0;
    int totrings = 0;
    int totelectrons = 0;

    auto increment = xrt::kernel(device, uuid, "increment");

    auto buffer_input = xrt::bo(device, vector_size_bytes, increment.group_id(0));
    auto buffer_output = xrt::bo(device, vector_size_bytes, increment.group_id(0));


    for (size_t idx = 0; idx < listfiles.size(); ++idx) {
        std::ifstream loadfile(listfiles[idx]);
        if (!loadfile.is_open()) {
            std::cerr << "Failed to open file: " << listfiles[idx] << std::endl;
            continue;
        }

       

        std::string line;
        std::vector<int> rings_array(4, 0);
        
        while (std::getline(loadfile, line)) {
            std::istringstream iss(line);
            std::vector<std::string> linedata((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
            
            if (linedata.size() < 10) continue; // Skip lines that are too short

            int type = std::stoi(linedata[3]);

            if (type == 20) {
                // Process RICH ring data
                float x = std::stof(linedata[5]);
                float y = std::stof(linedata[6]);
                float r = std::stof(linedata[7]);
            }
          
          
            if (type == 23) {
                // Process summary line with from RICH reco
                std::string id_ev = (linedata[0])
                int nt = std::stoi(linedata[4]);
                int hit = std::stoi(linedata[9]);
                if (hit > MAXHIT) hit = MAXHIT;
                //std::vector<int> elist(ehit,0);

                auto list = buffer_input.map<int*>();
                auto ker_list = buffer_output.map<int*>();
                
                for (int f = 10; f < (10 + hit), ++f){
                    list[f-10]= std::stoi(linedata[f]);                    
                } 

            rings_array[0] = x;
            rings_array[1] = y;
            rings_array[2] = r;
            rings_array[3] = nt;
            
//INSERIRE COUT CON GLI ELEMENTI DI HIT PER VEDERE SE LEGGE BENE
//NON CHIARO COME FARE CON BUFFER INOUT E BUFFER OUTPUT (FORSE COME HO FATTO E' OK)

            // Copy input data to device global memory
            std::cout << "Copying data..." << std::endl;
            buffer_input.sync(XCL_BO_SYNC_BO_TO_DEVICE);

            // Launch the Kernel
            std::cout << "Launching Kernel..." << std::endl;
            auto increment_run = xrt::run(mem_write);
            increment_run.set_arg(0, buffer_input);
            increment_run.set_arg(1, buffer_output);
            increment_run.start();


            increment_run.wait();

    // Copy Result from Device Global Memory to Host Local Memory
            std::cout << "Getting Results..." << std::endl;
            buffer_output.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

            for(int h = 0; h < hit; ++h){
                std::cout << "RAW-> " << list[h] << " | " << ker_list[h] << "<-INCR" << std::endl;                
                }            
            }
        }
    }
}





int main() {

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


    
    std::vector<std::string> listfiles = {"file1.txt", "file2.txt"}; // Example file list
    std::vector<std::vector<int>> hitList;
    std::vector<int> labelList;

    
    read_data_formatRECO2(listfiles, hitList, labelList);

    return 0;
}
