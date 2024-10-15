#include <vector>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <algorithm>

#include "grrm2xtb.hpp"
#include "utils.hpp"


namespace fs = std::filesystem;


GRRMInputData read_grrm_input(const fs::path input_file){
    // open input file with check
    std::ifstream file(input_file);
    if (!file.is_open()) {
            throw_error(input_file.string() + " not found.");
    }

    // read file
    std::vector<std::string> input_row_data;
    std::string line;
    while (std::getline(file, line)) {
        input_row_data.push_back(line);
    }

    struct GRRMInputData grrm_input_data;

    // TASK line
    std::string task_string = trim(split(input_row_data[0], ':')[1]);
    if (task_string == "MAKE GUESS") {
        grrm_input_data.task = "guess";
    } else if (task_string == "MICROITERATION") {
        grrm_input_data.task = "mi";
    } else if (task_string == "ENERGY") {
        grrm_input_data.task = "e";
    } else if (task_string == "ENERGY and GRADIENT") {
        grrm_input_data.task = "eg";
    } else if (task_string == "ENERGY, GRADIENT, and HESSIAN") {
        grrm_input_data.task = "egh";
    } else {
        throw_error(input_file.string() + " contains unexpected task type: " + task_string);
    }

    // NACTIVEATOM / NATOM: M / N
    std::string atom_data = split(input_row_data[3], ':')[1];
    std::vector<std::string> atom_values = split(atom_data, '/');
    grrm_input_data.num_activation_atom = std::stoi(trim(atom_values[0]));
    grrm_input_data.num_atom = std::stoi(trim(atom_values[1]));

    // atom coordinates
    for (int i = 0; i < grrm_input_data.num_atom; ++i) {
        grrm_input_data.atom_coordinates.push_back(replace_tab(input_row_data[4 + i]));
    }

    // NFROZENATOM: N
    grrm_input_data.num_frozen_atom = std::stoi(trim(split(input_row_data[4 + grrm_input_data.num_atom], ':')[1]));

    // frozen atom coordinates
    if (grrm_input_data.num_frozen_atom > 0) {
        for (int i = 0; i < grrm_input_data.num_frozen_atom; ++i) {
            grrm_input_data.frozen_atom_coordinates.push_back(replace_tab(input_row_data[5 + grrm_input_data.num_atom + i]));
        }
    }

    return grrm_input_data;
}


// Resize Hessian for Frozen Atoms
std::vector<std::vector<std::string>> resize_hessian(const std::vector<std::vector<std::string>>& hessian, int num_atom) {
    int size_n = num_atom * 3;

    // In case Hessian is not larger than required: return as is. 
    if (size_n >= hessian.size()) {
        return hessian;
    }

    // Resize Hessian
    std::vector<std::vector<std::string>> resized_hessian;
    for (int i = 0; i < size_n; ++i) {
        std::vector<std::string> resized_line(hessian[i].begin(), hessian[i].begin() + size_n);
        resized_hessian.push_back(resized_line);
    }

    return resized_hessian;
}

// Resize Gradient for Frozen Atoms
std::vector<std::vector<std::string>> resize_gradient(const std::vector<std::vector<std::string>>& gradient, int num_atom) {
    // In case Gradient is not larger than required: return as is. 
    if (num_atom >= gradient.size()) {
        return gradient;
    }

    // Resize Gradient
    return std::vector<std::vector<std::string>>(gradient.begin(), gradient.begin() + num_atom);
}

// Format Hessian for GRRM
std::vector<std::string> convert_hessian_to_grrm(const std::vector<std::vector<std::string>>& hessian) {
    int num_rows = hessian.size();
    int num_block = (num_rows % 5 == 0) ? (num_rows / 5) : (num_rows / 5 + 1);
    
    std::vector<std::string> formatted_hessian;

    for (int block = 0; block < num_block; ++block) {
        for (int row = 5 * block; row < num_rows; ++row) {
            std::ostringstream line;
            for (int col = 5 * block; col < std::min({5 * block + 5, num_rows, row + 1}); ++col) {
                line << std::setw(16) << hessian[row][col];
            }
            formatted_hessian.push_back(line.str() + "\n");
        }
    }

    return formatted_hessian;
}

// Format Gradient for GRRM
std::vector<std::string> convert_gradient_to_grrm(const std::vector<std::vector<std::string>>& gradient) {
    std::vector<std::string> grrm_gradient;

    for (const auto& line : gradient) {
        for (const auto& x : line) {
            grrm_gradient.push_back("  " + x + "\n");
        }
    }

    return grrm_gradient;
}

// Return Dummy Hessian for GRRM
std::vector<std::string> get_dummy_hessian_grrm(int num_atom) {
    int size_n = num_atom * 3;
    std::vector<std::vector<std::string>> hessian(size_n, std::vector<std::string>(size_n, "0.000000000000"));
    return convert_hessian_to_grrm(hessian);
}

// Return Dummy Gradient for GRRM
std::vector<std::string> get_dummy_gradient_grrm(int num_atom) {
    std::vector<std::string> gradient(num_atom * 3, "  0.000000000000\n");
    return gradient;
}

// Return Dummy Dipole Derivatives for GRRM
std::vector<std::string> get_dummy_dipole_derivatives(int num_atom) {
    std::vector<std::string> dipole_derivatives(num_atom * 3, "  0.000000000000  0.000000000000  0.000000000000\n");
    return dipole_derivatives;
}

