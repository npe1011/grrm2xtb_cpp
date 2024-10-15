
#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

// GRRM file name-related constants
inline const char* GRRM_INPUT_SUFFIX = "_INP4GEN.rrm";
inline const char* GRRM_OUTPUT_SUFFIX = "_OUT4GEN.rrm";

// XTB file name-related constants
inline const char* XTB_COMMAND = "xtb";
inline const char* XTB_INPUT_XYZ_FILE = "input.xyz";
inline const char* XTB_CONSTRAIN_FILE = "constrain.inp";
inline const char* XTB_ENERGY_FILE = "energy";
inline const char* XTB_GRADIENT_FILE = "gradient";
inline const char* XTB_HESSIAN_FILE = "hessian";
inline const char* XTB_OPT_XYZ_FILE = "xtbopt.xyz";
inline const char* XTB_LOG_FILE = "xtblog.log";

// Name for environmental variables for XTB Settings
inline const char* XTB_CHARGE_ENV = "XTB_CHARGE";
inline const char* XTB_MULTI_ENV = "XTB_MULTI";
inline const char* XTB_SOLVATION_ENV = "XTB_SOLVATION";
inline const char* XTB_SOLVENT_ENV = "XTB_SOLVENT";
inline const char* XTB_PARAM_ENV = "XTB_PARAM";
inline const char* XTB_SCRATCH_DIR_ENV = "XTB_SCRATCH_DIR";
inline const char* XTB_KEEP_LOG_ENV = "XTB_KEEP_LOG";

struct GRRMInputData {
    std::string task;
    int num_activation_atom;
    int num_atom;
    std::vector<std::string> atom_coordinates;
    int num_frozen_atom;
    std::vector<std::string> frozen_atom_coordinates;
    // Constructor
    GRRMInputData()
        : task(""),
          num_activation_atom(0),
          num_atom(0),
          atom_coordinates{}, 
          num_frozen_atom(0),
          frozen_atom_coordinates{} 
          {}  
};

/////////////////////////
// Defined in grrm.cpp //
/////////////////////////

// Resize Hessian for Frozen Atoms
GRRMInputData read_grrm_input(const fs::path);

// Resize Hessian for Frozen Atoms
std::vector<std::vector<std::string>> resize_hessian(const std::vector<std::vector<std::string>>&, int);

// Resize Gradient for Frozen Atoms
std::vector<std::vector<std::string>> resize_gradient(const std::vector<std::vector<std::string>>&, int);

// Format Hessian for GRRM
std::vector<std::string> convert_hessian_to_grrm(const std::vector<std::vector<std::string>>&);

// Format Gradient for GRRM
std::vector<std::string> convert_gradient_to_grrm(const std::vector<std::vector<std::string>>&);

// Return Dummy Hessian for GRRM
std::vector<std::string> get_dummy_hessian_grrm(int);

// Return Dummy Gradient for GRRM
std::vector<std::string> get_dummy_gradient_grrm(int);

// Return Dummy Dipole Derivatives for GRRM
std::vector<std::string> get_dummy_dipole_derivatives(int);

/////////////////////////
// Defined in xtb.cpp  //
/////////////////////////

// Read XTB Hessian File
std::vector<std::vector<std::string>> read_hessian(const std::string&, int);

// Read XTB Energy File
std::string read_energy(const std::string&);

// Read XTB Grad File
std::vector<std::vector<std::string>> read_gradient(const std::string&, int);

// Read XYZ file
std::vector<std::string> read_xyz(const std::string&);

// Prepare XYZ file GRRMInputData (Atoms + FrozenAtoms)
void prepare_xyz_file(const GRRMInputData&, const fs::path&);

// Prepare Constrain file for XTB from GRRMInputData. Return true when fix is required.
bool prepare_constrain_file(const GRRMInputData&, const fs::path&);

// Run XTB (in current directory)
void run_xtb(const GRRMInputData&);
