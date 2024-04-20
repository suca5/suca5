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
                int hypo = std::stoi(linedata[4]);
                float x = std::stof(linedata[5]);
                float y = std::stof(linedata[6]);
                float r = std::stof(linedata[7]);
                int rhit = std::stoi(linedata[9]);
                std::vector<int> rlist(linedata.begin() + 10, linedata.begin() + 10 + rhit);

                // Debug print
                // std::cout << "RING: hyp " << hypo << " X " << x << " Y " << y << " R " << r << " Nhit " << rhit << std::endl;
                // for (int hit : rlist) std::cout << hit << " ";
                // std::cout << std::endl;
            } else if (type == 23) {
                // Process summary line with from RICH reco
                int nt = std::stoi(linedata[4]);
                int el = std::stoi(linedata[5]);
                int mu = std::stoi(linedata[6]);
                int pi = std::stoi(linedata[7]);
                int ka = std::stoi(linedata[8]);
                int ehit = std::stoi(linedata[9]);
                if (ehit > MAXHIT) ehit = MAXHIT;
                std::vector<int> elist(linedata.begin() + 10, linedata.begin() + 10 + ehit);

                // Prepare feature list
                for (int i = 0; i < ehit; ++i) {
                    hits_array[i] = elist[i];
                }

                // Update counters
                nevt += 1;
                nrings += nt;
                nelectrons += mu;

                // Debug print
                // std::cout << "EVENT: nTracks: " << nt << "(el " << el << " mu " << mu << " pi " << pi << " ka " << ka << ") Nhit " << ehit << std::endl;

                // Write features and labels on output vectors
                hitList.push_back(hits_array);
                labelList1.push_back(nt);
                labelList2.push_back(el);

                // Check enough statistics was collected
                if (nevt >= samples) {
                    totevt += nevt;
                    totrings += nrings;
                    totelectrons += nelectrons;
                    std::cout << "file[" << idx << "] evt = " << nevt << " rings = " << nrings << " electrons = " << nelectrons << std::endl;
                    break;
                }
            }
        }
    }

    std::cout << "tot events = " << totevt << std::endl;
    std::cout << "tot rings = " << totrings << std::endl;
    std::cout << "tot electrons = " << totelectrons << std::endl;
}

int main() {
    std::vector<std::string> listfiles = {"file1.txt", "file2.txt"}; // Example file list
    std::vector<std::vector<int>> hitList;
    std::vector<int> labelList1, labelList2;
    int samples = 100; // Example samples

    read_data_formatRECO2(listfiles, hitList, labelList1, labelList2, samples);

    return 0;
}
