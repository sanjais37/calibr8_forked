#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <vector>

// Function to replace a string in a file (simple implementation)
void replaceStringInFile(const std::string& filePath, const std::string& searchString, const std::string& replaceString) {
    std::ifstream fileIn(filePath);
    std::string fileContent((std::istreambuf_iterator<char>(fileIn)), std::istreambuf_iterator<char>());
    fileIn.close();

    size_t pos = fileContent.find(searchString);
    while (pos != std::string::npos) {
        fileContent.replace(pos, searchString.length(), replaceString);
        pos = fileContent.find(searchString, pos + replaceString.length());
    }

    std::ofstream fileOut(filePath);
    fileOut << fileContent;
    fileOut.close();
}

// Function to run the YAML file with current parameters
void runYamlWithParameters(const std::string& yamlFilePath) {
    std::string command = "time primal " + yamlFilePath;
    system(command.c_str());
}

int main() {
    // Read the .text file
    std::ifstream inputFile("matData_set.text");
    std::string line;
    std::vector<std::vector<std::string>> data;
    
    std::cout << "Hello, World_1!" << std::endl;
    
    while (std::getline(inputFile, line)) {
        std::istringstream ss(line);
        std::string item;
        std::vector<std::string> row;

        while (std::getline(ss, item, '\t')) {
            row.push_back(item);
        }
        data.push_back(row);
    }
    inputFile.close();
    
    std::cout << "Hello, World_2!" << std::endl;

    // Loop through the 10 sets of values
    for (size_t i = 1; i < data[0].size(); ++i) {  // Assuming the first row is labels
        std::cout << "Hello, World_1!" << std::endl;
        std::string E = data[1][i];
        std::string nu = data[2][i];
        std::string Y = data[3][i];
        std::string S = data[4][i];
        std::string D = data[5][i];
        
        std::cout << "Hello, World_3!" << std::endl;

        // Replace the values in the YAML file
        replaceStringInFile("notch2D_Asym_small_J2_plane_stress.yaml", "E: 200.e3", "E: " + E);
        replaceStringInFile("notch2D_Asym_small_J2_plane_stress.yaml", "nu: 0.30", "nu: " + nu);
        replaceStringInFile("notch2D_Asym_small_J2_plane_stress.yaml", "Y: 330.", "Y: " + Y);
        replaceStringInFile("notch2D_Asym_small_J2_plane_stress.yaml", "S: 1000.", "S: " + S);
        replaceStringInFile("notch2D_Asym_small_J2_plane_stress.yaml", "D: 2.5", "D: " + D);
	
	std::cout << "Hello, World_4!" << std::endl;
	
        // Run the YAML file with current parameters
        runYamlWithParameters("notch2D_Asym_small_J2_plane_stress.yaml");
    }

    return 0;
}

