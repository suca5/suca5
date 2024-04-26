#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

const int MAXHIT = 64;

void read_data_formatRECO2(const std::vector<std::string>& listfiles, std::vector<std::vector<int>>& hitList, std::vector<int>& labelList1, std::vector<int>& labelList2, int samples) {
    bool p = false;
    int totevt = 0;
    int totrings = 0;
    int totelectrons = 0;

    for (size_t idx = 0; idx < listfiles.size(); ++idx) {
        std::ifstream loadfile(listfiles[idx]);
        if (!loadfile.is_open()) {
            std::cerr << "Impossibile aprire il file " << listfiles[idx] << std::endl;
            continue;
        }

        int nevt = 0;
        int nrings = 0;
        int nelectrons = 0;

        std::vector<int> hits_array(MAXHIT, 0);

        std::string line;
        while (std::getline(loadfile, line)) {
            std::istringstream iss(line);
            std::vector<std::string> linedata(std::istream_iterator<std::string>{iss},
                                              std::istream_iterator<std::string>());

            if (std::stoi(linedata[3]) == 20) { // RICH ring data
                int hypo = std::stoi(linedata[4]);
                double x = std::stod(linedata[5]);
                double y = std::stod(linedata[6]);
                double r = std::stod(linedata[7]);
                int rhit = std::stoi(linedata[9]);
                // ...

                if (p) {
                    std::cout << "RING: hyp " << hypo << " X " << x << " Y " << y << " R " << r << " Nhit " << rhit << std::endl;
                }
            }

            if (std::stoi(linedata[3]) == 23) { // Summary line with from RICH reco
                int nt = std::stoi(linedata[4]);
                int el = std::stoi(linedata[6]); // Column 6 has n electron from custom definition
                int ehit = std::stoi(linedata[9]);
                if (ehit > MAXHIT) {
                    ehit = MAXHIT;
                }
                // ...

                for (int i = 0; i < ehit; ++i) {
                    hits_array[i] = std::stoi(linedata[10 + i]);
                }

                nevt++;
                nrings += nt;
                nelectrons += el;

                if (p) {
                    std::cout << "EVENT: nTracks: " << nt << " (el " << el << ") Nhit " << ehit << std::endl;
                }

                hitList.push_back(hits_array);
                labelList1.push_back(nt);
                labelList2.push_back(el);

                if (nevt >= samples) {
                    totevt += nevt;
                    totrings += nrings;
                    totelectrons += nelectrons;
                    std::cout << "file[" << idx << "] evt = " << nevt << " rings = " << nrings << " electrons = " << nelectrons << std::endl;
                    break;
                } else {
                    hits_array.assign(MAXHIT, 0);
                }
            }
        }

        loadfile.close();
    }

    std::cout << "tot events = " << totevt << std::endl;
    std::cout << "tot rings = " << totrings << std::endl;
    std::cout << "tot electrons = " << totelectrons << std::endl;
}

int main() {
    std::vector<std::string> listfiles = {"file1.txt", "file2.txt", "file3.txt"}; // List of files
    std::vector<std::vector<int>> hitList;
    std::vector<int> labelList1;
    std::vector<int> labelList2;
    int samples = 10; // Number of samples to read from each file

    read_data_formatRECO2(listfiles, hitList, labelList1, labelList2, samples);

    return 0;
}
