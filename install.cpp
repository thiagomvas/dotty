#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "utils.h" // Include utility functions
using namespace std;
namespace fs = std::filesystem;

// Function to install packages based on the package manager
void install_packages(ifstream& packages, bool testRun) {
    string package_manager;
    string line;
    info("Installing packages...");

    // Find the package manager from the file
    while (getline(packages, line)) {
        if (line.find("Package Manager:") != string::npos) {
            package_manager = line.substr(line.find(":") + 2);
            break;
        }
    }

    if (package_manager.empty()) {
        error("No package manager found in the .packages file.");
        return;
    }

    // Output the detected package manager
    success("Using package manager: " + package_manager);

    // Depending on the package manager, run the corresponding install commands
    while (getline(packages, line)) {
        if (line.empty() || line == "done") continue; // Skip empty lines or "done" marker

        if (testRun) {
            success("Test Mode: Would install package: " + line);
        } else {
            string install_command;
            if (package_manager == "apt") {
                install_command = "sudo apt install -y " + line;
            } else if (package_manager == "dnf") {
                install_command = "sudo dnf install -y " + line;
            } else if (package_manager == "pacman") {
                install_command = "sudo pacman -S --noconfirm " + line;
            } else if (package_manager == "yum") {
                install_command = "sudo yum install -y " + line;
            } else if (package_manager == "zypper") {
                install_command = "sudo zypper install -y " + line;
            } else if (package_manager == "brew") {
                install_command = "brew install " + line;
            } else {
                error("Unsupported package manager: " + package_manager);
                return;
            }

            success("Running command: " + install_command);
            system(install_command.c_str());
        }
    }
}

void symlink_configs(const fs::path& configRoot, const fs::path& home, bool testMode) {
    info("Creating symlinks for configuration files...");

    // Iterate over the contents of configRoot
    for (const auto& entry : fs::recursive_directory_iterator(configRoot, fs::directory_options::skip_permission_denied)) {

        // If it's a directory, symlink the directory
        if (entry.is_directory()) {
            // Get the relative path of the directory
            fs::path relative = fs::relative(entry.path(), configRoot);
            fs::path home_path = home / relative;

            if (testMode) {
                info("Would create symlink directory: " + entry.path().string() + " → " + home_path.string());
            } else {
                try {
                    fs::create_directories(home_path); // Ensure the directory exists in home
                    if (!fs::exists(home_path)) {
                        fs::create_symlink(entry.path(), home_path); // Create symlink for the directory
                        success("Created symlink directory: " + entry.path().string() + " → " + home_path.string());
                    }
                } catch (const std::exception& e) {
                    error("Failed to create symlink for directory: " + entry.path().string() + " → " + home_path.string() + " | Error: " + e.what());
                }
            }
        }

        // If it's a file, symlink the file
        else if (entry.is_regular_file()) {
            // Get the relative path for the file
            fs::path relative = fs::relative(entry.path(), configRoot);
            fs::path home_path = home / relative;

            if (testMode) {
                info("Would create symlink file: " + entry.path().string() + " → " + home_path.string());
            } else {
                fs::create_directories(home_path.parent_path()); // Ensure the parent directory exists

                try {
                    if (fs::exists(home_path)) {
                        // If it exists, create a backup by renaming it with .bak extension
                        fs::path backup_path = home_path;
                        backup_path.replace_extension(".bak");  // Change the file extension to .bak

                        // Rename the existing file or symlink to a backup
                        fs::rename(home_path, backup_path);
                        info("Backed up existing file or symlink to: " + backup_path.string());
                    }
                    fs::create_symlink(entry.path(), home_path); // Create symlink for the file
                    success("Created symlink file: " + entry.path().string() + " → " + home_path.string());
                } catch (const std::exception& e) {
                    error("Failed to create symlink for file: " + entry.path().string() + " → " + home_path.string() + " | Error: " + e.what());
                }
            }
        }
    }

    success("Symlinks creation process completed.");
}

void execute_scripts(const fs::path& scripts_dir) {
    info("Executing scripts...");

    // Iterate over the contents of the scripts directory
    for (const auto& entry : fs::directory_iterator(scripts_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".sh") {
            string command = "bash " + entry.path().string();
            success("Running script: " + command);
            system(command.c_str());
        }
    }

    success("Script execution process completed.");
}


// Main function
int main(int argc, char *argv[]) {
    bool testRun = false;

    // Check if test mode is enabled
    if (argc > 1 && (strcmp(argv[1], "--test") == 0 || strcmp(argv[1], "-t") == 0)) {
        testRun = true;
        info("Test Mode enabled.");
    }

    // Set root path and home path
    auto root = fs::current_path();
    auto home = fs::path(getenv("HOME"));

    // Path to the .packages file
    fs::path packages_file = root / ".packages";
    if (fs::exists(packages_file)) {
        // Open the .packages file
        ifstream pkg_file(packages_file);
        if (!pkg_file.is_open()) {
            error("Failed to open .packages file!");
            return 1;
        }

        // Start the installation process
        install_packages(pkg_file, testRun);
    }


    symlink_configs(root / "config-backup", home, testRun);

    execute_scripts(root / "scripts");


    success("Installation process completed.");

    return 0;
}
