#include <iostream>
#include "utils.h"  // Include the utility functions
#include <fstream>  // For file handling
using namespace std;

namespace fs = std::filesystem;

void setup_packages(filesystem::path root) {
    fs::path packages_file = root / ".packages";

    // Check if .packages file exists
    if (fs::exists(packages_file)) {
        prompt(".packages file already exists. Do you want to (o)verride or (a)dd more packages? (press 'enter' to skip):");
        char choice = ask_input()[0];

        if (choice == 'o' || choice == 'O') {
            // Override the file
            ofstream pkg_file(packages_file, ios::trunc);
            if (!pkg_file.is_open()) {
                error("Failed to open .packages file for overwriting!");
            }
            success(".packages file will be overwritten.");

            // Proceed to collect package manager and package names
            string package_manager = setup_package_manager();
            pkg_file << "Package Manager: " << package_manager << endl;
            collect_package_names(pkg_file);

            success("Package names saved to .packages.");
        } else if (choice == 'a' || choice == 'A') {
            // Append to the file
            ofstream pkg_file(packages_file, ios::app);
            if (!pkg_file.is_open()) {
                error("Failed to open .packages file for appending!");
            }
            success(".packages file will be appended.");

            // Collect package names and append them
            collect_package_names(pkg_file);

            success("Package names appended to .packages.");
        }
    } else {
        // If the file doesn't exist, create a new one
        prompt("Do you want to create a new .packages file? (y/n): ");
        char choice = ask_input()[0];

        if (choice == 'y' || choice == 'Y') {
            ofstream pkg_file(packages_file);
            if (!pkg_file.is_open()) {
                error("Failed to create .packages file!");
            }

            success(".packages file created successfully!");

            string package_manager = setup_package_manager();
            pkg_file << "Package Manager: " << package_manager << endl;
            collect_package_names(pkg_file);

            success("Package names saved to .packages.");
        }
    }
}

// Function to collect the package manager

void export_configs(const fs::path& root, const fs::path& home) {
    prompt("Do you want to export configuration files to dotfiles repo? (y/n): ");
    char choice = ask_input()[0];

    if (choice == 'y' || choice == 'Y') {
        fs::path dotfiles_config_dir = root / "config-backup";
        fs::create_directories(dotfiles_config_dir);

        auto selected_configs = select_configs(home);

        for (const auto& config : selected_configs) {
            if (!fs::exists(config)) {
                error("Not found: " + config.string());
                continue;
            }

            // Get relative path from home directory
            fs::path relative = fs::relative(config, home);
            fs::path dest = dotfiles_config_dir / relative;

            // Create parent directories in backup
            fs::create_directories(dest.parent_path());

            // Copy file or directory
            fs::copy(config, dest, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
            success("Copied: " + config.string() + " → " + dest.string());
        }

        success("All selected configs exported with relative structure.");
    }
}
void collect_post_install_commands(const fs::path& root) {
    prompt("Do you want to add post-install commands? (y/n): ");
    char choice = ask_input()[0];
    if (choice != 'y' && choice != 'Y') return;

    fs::path scripts_dir = root / "scripts";

    fs::path post_install_file = scripts_dir / "post-install.sh";
    ofstream script_file(post_install_file, ios::trunc);

    if (!script_file.is_open()) {
        error("Failed to create post-install.sh script file.");
        return;
    }

    success("Enter post-install shell commands (type 'done' to finish):");

    while (true) {
        string command = ask_input();
        if (command == "done") break;
        script_file << command << '\n';
    }

    script_file.close();
    fs::permissions(post_install_file, fs::perms::owner_exec | fs::perms::owner_write | fs::perms::owner_read);

    success("Post-install commands saved to scripts/post-install.sh");
}

int main() {
    auto root = fs::current_path();
    auto home = get_home();

    // Ask if the user wants to create a .packages file
    setup_packages(root);
    export_configs(root, home);
    fs::path scripts_dir = root / "scripts";
    fs::create_directories(scripts_dir);
    collect_post_install_commands(root);

    // Inform what the scrips folder is
    info("The 'scripts/' folder contains post-install commands and other shell scripts that are ran after the program is installed.");

    success("Setup completed successfully!");
    return 0;
}
