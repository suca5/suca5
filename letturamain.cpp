#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

const int MAXHIT = 64;
const int NLABEL = 4;

void read_data_formatRECO2(std::vector<std::string> listfiles, std::vector<std::vector<int>>& hitList, std::vector<int>& labelList1, std::vector<int>& labelList2, int samples) {
    int totevt = 0;
    int totrings = 0;
    int totelectrons = 0;

    for (size_t idx = 0; idx < listfiles.size(); ++idx) {
        std::ifstream loadfile(listfiles[idx]);
        if (!loadfile.is_open()) {
            std::cerr << "Failed to open file: " << listfiles[idx] << std::endl;
            continue;
        }

        std::vector<int> hits_array(MAXHIT, 0);
        int nevt = 0;
        int nrings = 0;
        int nelectrons = 0;

        std::string line;
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
                int ehit = std::stoi(linedata[9]);
                if (ehit > MAXHIT) ehit = MAXHIT;
                std::vector<int> elist(ehit,0)
                
                for (int f = 10; f < (10 + ehit), ++f){
                    elist[f-10]= std::stoi(linedata[f]);                    
                } 

                // Prepare feature list
                for (int i = 0; i < ehit; ++i) {
                    hits_array[i] = elist[i];
                }

                // Update counters
                nevt += 1;
                nrings += nt;
          
                // Write features and labels on output vectors
                hitList.push_back(hits_array);
                labelList1.push_back(nt);
          
            }
        }
    }

    std::cout << "tot events = " << totevt << std::endl;
    std::cout << "tot rings = " << totrings << std::endl;
    //std::cout << "tot electrons = " << totelectrons << std::endl;
}

int main() {
    std::vector<std::string> listfiles = {"file1.txt", "file2.txt"}; // Example file list
    std::vector<std::vector<int>> hitList;
    std::vector<int> labelList1, labelList2;
    int samples = 100; // Example samples

    read_data_formatRECO2(listfiles, hitList, labelList1, labelList2, samples);

    return 0;
}
