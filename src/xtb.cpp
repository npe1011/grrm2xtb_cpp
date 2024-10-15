#include <fstream>
#include <cstring>

#include "grrm2xtb.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;


// Read XTB Hessian File
std::vector<std::vector<std::string>> read_hessian(const std::string& hessian_file, int num_atom) {
    // Open and check file
    std::ifstream file(hessian_file);
    if (!file.is_open()) {
        throw_error(hessian_file + " not found.");
    }

    // Read file
    std::vector<std::string> hessian_data;
    std::string line;
    while (std::getline(file, line)) {
        hessian_data.push_back(line);
    }

    // Sanity check
    if (hessian_data[0].find("$hessian") != 0) {
        throw_error("Invalid hessian file: missing $hessian at the beginning.");
    }

    std::vector<std::string> hess_1d;
    for (size_t i = 1; i < hessian_data.size(); ++i) {
        std::string trimmed_line = trim(hessian_data[i]);

        // Skip blank line
        if (trimmed_line.empty()) {
            continue;
        }

        // $end > stop reading
        if (trimmed_line.find("$end") == 0) {
            break;
        }

        // split a line with blank chars
        std::vector<std::string> split_line = split_by_blank(trimmed_line);
        hess_1d.insert(hess_1d.end(), split_line.begin(), split_line.end());
    }

    // Sanity check
    size_t expected_size = static_cast<size_t>(num_atom * 3) * static_cast<size_t>(num_atom * 3);
    if (hess_1d.size() != expected_size) {
        throw_error("Hessian data size mismatch: expected " + std::to_string(expected_size) + ", got " + std::to_string(hess_1d.size()));
    }

    // Convert 1D vector to 2D data (matrix)
    std::vector<std::vector<std::string>> hessian_matrix;
    for (size_t i = 0; i < hess_1d.size(); i += num_atom * 3) {
        std::vector<std::string> row(hess_1d.begin() + i, hess_1d.begin() + i + num_atom * 3);
        hessian_matrix.push_back(row);
    }

    return hessian_matrix;
}

// Read XTB Energy File
std::string read_energy(const std::string& energy_file) {
    // Open and check file
    std::ifstream file(energy_file);
    if (!file.is_open()) {
        throw_error(energy_file + " not found.");
    }

    // Read file
    std::vector<std::string> energy_data;
    std::string line;
    while (std::getline(file, line)) {
        energy_data.push_back(line);
    }

    // Sanity check
    if (energy_data[0].find("$energy") != 0) {
        throw_error("Invalid energy file: missing $energy at the beginning.");
    }

    // Get energy value
    return split_by_blank(energy_data[1])[1];
}

// Read XTB Grad File
std::vector<std::vector<std::string>> read_gradient(const std::string& gradient_file, int num_atom) {
    // Open and check file
    std::ifstream file(gradient_file);
    if (!file.is_open()) {
        throw_error(gradient_file + " not found.");
    }

    // Read file
    std::vector<std::string> gradient_data;
    std::string line;
    while (std::getline(file, line)) {
        gradient_data.push_back(line);
    }

    // Sanity check
    if (gradient_data[0].find("$grad") != 0) {
        throw_error("Invalid gradient file: missing $grad at the beginning.");
    }

    std::vector<std::vector<std::string>> gradient;
    // read gradient data
    for (size_t i = 2 + num_atom; i < 2 + 2 * num_atom; ++i) {
        gradient.push_back(split_by_blank(gradient_data[i]));
    }

    return gradient;
}

// Read XYZ file
std::vector<std::string> read_xyz(const std::string& xyz_file) {
    // Open file with check
    std::ifstream file(xyz_file);
    if (!file.is_open()) {
        throw_error(xyz_file + " not found.");
    }

    // Read file
    std::vector<std::string> xyz_data;
    std::string line;
    while (std::getline(file, line)) {
        xyz_data.push_back(line);
    }

    // atom number
    int num_atom = std::stoi(xyz_data[0]);

    // return coordinates
    std::vector<std::string> coordinates(xyz_data.begin() + 2, xyz_data.begin() + 2 + num_atom);
    return coordinates;
}

// Prepare XYZ file GRRMInputData (Atoms + FrozenAtoms)
void prepare_xyz_file(const GRRMInputData& input_data, const fs::path& xyz_file) {
    int num_atom = input_data.num_atom + input_data.num_frozen_atom;
    
    // Open file and check
    std::ofstream ofs(xyz_file);
    if (!ofs) {
        throw_error("Failed to open file: " + xyz_file.string());
    }

    // Frist 2 lines
    ofs << num_atom << "\ncoord\n";

    // Write Atoms
    for (const auto& coord : input_data.atom_coordinates) {
        ofs << coord << std::endl;
    }

    // Write Frozen Atoms
    if (input_data.num_frozen_atom > 0) {
        for (const auto& frozen_coord : input_data.frozen_atom_coordinates) {
            ofs << frozen_coord << std::endl;
        }
    }

    ofs.close();
}

// Prepare Constrain file for XTB from GRRMInputData. Return true when fix is required.
bool prepare_constrain_file(const GRRMInputData& input_data, const fs::path& constrain_file) {
    // Case: MicroIteration
    if (input_data.task == "mi") {
        std::ofstream ofs(constrain_file);
        if (!ofs) {
            throw_error("Failed to open file: " + constrain_file.string());
        }

        ofs << "$fix\n";

        // Fix for micro iteration
        int start_freeze = 1;
        int end_freeze = input_data.num_activation_atom;
        if (start_freeze != end_freeze) {
            ofs << "    atoms: " << start_freeze << "-" << end_freeze;
        } else {
            ofs << "    atoms: " << start_freeze;
        }

        // Fix for frozen atoms
        if (input_data.num_frozen_atom > 0) {
            start_freeze = input_data.num_atom + 1;
            end_freeze = start_freeze + input_data.num_frozen_atom - 1;
            if (start_freeze != end_freeze) {
                ofs << ", " << start_freeze << "-" << end_freeze << "\n";
            } else {
                ofs << " " << start_freeze << "\n";
            }
        } else {
            ofs << "\n"; // No frozen atoms
        }

        ofs << "$end\n";
        return true;
    } else {
        // Fix for frozen atoms
        if (input_data.num_frozen_atom > 0) {
            std::ofstream ofs(constrain_file);
            if (!ofs) {
                throw_error("Failed to open file: " + constrain_file.string());
            }

            ofs << "$fix\n";
            int start_freeze = input_data.num_atom + 1;
            int end_freeze = start_freeze + input_data.num_frozen_atom - 1;
            if (start_freeze != end_freeze) {
                ofs << "    atoms: " << start_freeze << "-" << end_freeze << "\n";
            } else {
                ofs << "    atoms: " << start_freeze << "\n";
            }

            ofs << "$end\n";
            return true;
        } else {
            return false;
        }
    }
}

// Run XTB (in current directory)
void run_xtb(const GRRMInputData& input_data) {

    // Prepare xyz file and constrain file when required.
    prepare_xyz_file(input_data, XTB_INPUT_XYZ_FILE);
    bool constrain_flag = prepare_constrain_file(input_data, XTB_CONSTRAIN_FILE);

    // prep xtb command from here.
    std::vector<std::string> xtb_commands = {"xtb"};
    
    // opt job if micro iteration is called
    if (input_data.task == "mi") {
        xtb_commands.push_back("--opt");
    }
    if (constrain_flag) {
        xtb_commands.push_back("--input");
        xtb_commands.push_back(XTB_CONSTRAIN_FILE);
    }
    
    // charge
    std::string charge = "0";
    if (const char* xtb_charge = std::getenv(XTB_CHARGE_ENV)) {
        if (strlen(xtb_charge) > 0) {
            charge = std::string(xtb_charge);
        }
    }
    xtb_commands.push_back("--chrg");
    xtb_commands.push_back(charge);
    
    // mult, uhf
    std::string spin = "0";
    if (const char* xtb_multi = std::getenv(XTB_MULTI_ENV)) {
        if (strlen(xtb_multi) > 0) {
            spin = std::to_string(std::stoi(xtb_multi) - 1);
        }
    }
    xtb_commands.push_back("--uhf");
    xtb_commands.push_back(spin);
    
    // solvation and solvent
    bool solvation_flag = false;
    bool solvent_flag = false;
    std::string solvation, solvent;
    if (const char* env_solvation = std::getenv(XTB_SOLVATION_ENV)) {
        solvation = to_lowercase(std::string(env_solvation));
        if (solvation == "gbsa" || solvation == "alpb") {
            solvation_flag = true;
        } else {
            throw_error("Solvation is invalid. Use gbsa or alpb.");
        }
    }
    if (const char* env_solvent = std::getenv(XTB_SOLVENT_ENV)) {
        solvent = std::string(env_solvent);
        solvent_flag = !solvent.empty();
    }
    
    // sanity check and set command
    if (solvation_flag && solvent_flag) {
        xtb_commands.push_back("--" + solvation);
        xtb_commands.push_back(solvent);
    } else if (solvation_flag) {
        throw_error("Solvation is turned on but solvent is not specified.");
    } else if (solvent_flag) {
        throw_error("Solvation model is not selected but solvent is specified.");
    }

    // param (gfn1/2/ff)
    if (const char* xtb_param = std::getenv(XTB_PARAM_ENV)) {
        if (strlen(xtb_param) > 0) {
            xtb_commands.push_back("--gfn");
            xtb_commands.push_back(xtb_param);
        }
    }

    // task
    if (input_data.task == "egh") {
        xtb_commands.push_back("--hess");
        xtb_commands.push_back("--grad");
    } else if (input_data.task == "e" || input_data.task == "eg" || input_data.task == "mi") {
        xtb_commands.push_back("--grad");
    }

    xtb_commands.push_back(XTB_INPUT_XYZ_FILE);

    // Create a command string from the vector
    std::string command;
    for (const auto& cmd : xtb_commands) {
        command += cmd + " ";
    }

    // Run the command and redirect output
    int result = std::system((command + " > " + XTB_LOG_FILE + " 2>&1").c_str());
    if (result != 0) {
        throw_error("XTB command failed: " + command);
    }
}
