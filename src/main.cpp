#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <string>
#include <cstdlib>
#include <filesystem>

#include <cstdio>
#include <chrono>
#include <unistd.h>

#include "grrm2xtb.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;


int main(int argc, char* argv[]) {
    if (argc < 2) {
        throw_error("Job not provided");
    }

    std::string job_name = argv[1];
    fs::path orig_dir = fs::current_path();

    // path settings
    fs::path scratch_dir = fs::absolute(fs::path(std::getenv(XTB_SCRATCH_DIR_ENV) ? std::getenv(XTB_SCRATCH_DIR_ENV) : fs::current_path()));
    fs::path work_dir = scratch_dir / fs::path((job_name + "_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + "_" + std::to_string(getpid())));
    fs::path input_file = fs::absolute(fs::path(job_name + GRRM_INPUT_SUFFIX));
    fs::path output_file = fs::absolute(fs::path(job_name + GRRM_OUTPUT_SUFFIX));

    // prepare working directory
    try {
        fs::create_directories(work_dir);
    } catch (const std::exception& e) {
        throw_error("Failed to create working directory...");
    }

    GRRMInputData grrm_input_data = read_grrm_input(input_file);

    // GUESS is not available!
    if (grrm_input_data.task == "guess") {
        throw_error("TASK GUESS is unavailable with XTB.");
    }

    // Run XTB in working directory
    fs::current_path(work_dir);
    run_xtb(grrm_input_data);

    // Read data
    int full_num_atom = grrm_input_data.num_atom + grrm_input_data.num_frozen_atom;
    std::string energy; 
    std::vector<std::vector<std::string>> gradient;
    std::vector<std::vector<std::string>> hessian;
    std::vector<std::string> opt_coordinates;
    
    if (grrm_input_data.task == "mi") {
        opt_coordinates = read_xyz(XTB_OPT_XYZ_FILE);
        energy = read_energy(XTB_ENERGY_FILE);
        gradient = read_gradient(XTB_GRADIENT_FILE, full_num_atom);
    } else if (grrm_input_data.task == "e") {
        energy = read_energy(XTB_ENERGY_FILE);
    } else if (grrm_input_data.task == "eg") {
        energy = read_energy(XTB_ENERGY_FILE);
        gradient = read_gradient(XTB_GRADIENT_FILE, full_num_atom);
    } else if (grrm_input_data.task == "egh") {
        energy = read_energy(XTB_ENERGY_FILE);
        gradient = read_gradient(XTB_GRADIENT_FILE, full_num_atom);
        hessian = read_hessian(XTB_HESSIAN_FILE, full_num_atom);
    }

    // Return to the original job directory
    fs::current_path(orig_dir);

    // Prepare output file for GRRM
    std::ofstream output(output_file);
    if (!output) {
        throw_error("Failed to open output file.");
    }
    
    output << "RESULTS\n";
    output << "CURRENT COORDINATE\n";
    if (grrm_input_data.task == "mi") {
        for (int i = 0; i < grrm_input_data.num_atom; ++i) {
            output << opt_coordinates[i] << std::endl;
        }
    } else {
        for (const auto& line : grrm_input_data.atom_coordinates) {
            output << line << std::endl;
        }
    }
    output << "ENERGY =  " << energy << "  0.000000000000  0.000000000000\n";
    output << "       =  0.000000000000  0.000000000000  0.000000000000\n";
    output << "S**2   =  0.000000000000\n";
    output << "GRADIENT\n";

    std::vector<std::string> output_gradient;
    if (grrm_input_data.task == "mi" || grrm_input_data.task == "eg" || grrm_input_data.task == "egh") {
        std::vector<std::vector<std::string>> resized_gradient = resize_gradient(gradient, grrm_input_data.num_atom);
        output_gradient = convert_gradient_to_grrm(resized_gradient);
    } else {
        output_gradient = get_dummy_gradient_grrm(grrm_input_data.num_atom);
    }
    for (const auto& line : output_gradient) { output << line; }
    
    output << "DIPOLE =  0.000000000000  0.000000000000  0.000000000000\n";
    output << "HESSIAN\n";

    std::vector<std::string> output_hessian;
    if (grrm_input_data.task == "egh") {
        std::vector<std::vector<std::string>> resized_hessian = resize_hessian(hessian, grrm_input_data.num_atom);
        output_hessian = convert_hessian_to_grrm(resized_hessian);
    } else {
        output_hessian = get_dummy_hessian_grrm(grrm_input_data.num_atom);
    }
    for (const auto& line : output_hessian) { output << line; }

    output << "DIPOLE DERIVATIVES\n";
    std::vector<std::string> dipole_derivatives = get_dummy_dipole_derivatives(grrm_input_data.num_atom);
    for (const auto& line : dipole_derivatives) { output << line; }
    output << "POLARIZABILITY\n";
    output << "  0.000000000000\n";
    output << "  0.000000000000  0.000000000000\n";
    output << "  0.000000000000  0.000000000000  0.000000000000\n";

    output.close();

    // check flag to keep log files
    bool keep_log_flag = false;
    if (const char* xtb_keep_log = std::getenv(XTB_KEEP_LOG_ENV)) {
        std::string keep_log_value = to_lowercase(std::string(xtb_keep_log));
        if (keep_log_value == "true" || keep_log_value == "1" || keep_log_value == "on") {
            keep_log_flag = true;
        }
    }

    // remove logs and work dir
    if (!keep_log_flag) {
        fs::remove_all(work_dir); 
    }

    return 0;
}