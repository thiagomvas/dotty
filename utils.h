#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>

// ANSI color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

// Unicode symbols for decoration
#define CHECK_MARK "✔️"
#define CROSS_MARK "❌"
#define WARNING "⚠️"
#define INFORMATION "ℹ️"

using namespace std;
namespace fs = std::filesystem;

// Utility function to get the home directory
std::filesystem::path get_home() {
    const char* home = getenv("HOME");
    if (!home) throw std::runtime_error("HOME environment variable not set");
    return std::filesystem::path(home);
}


std::string get_package_manager() {
    // Check if `apt` is available (Debian/Ubuntu-based)
    if (system("which apt > /dev/null 2>&1") == 0) {
        return "apt";
    }
    // Check if `pacman` is available (Arch-based)
    if (system("which pacman > /dev/null 2>&1") == 0) {
        return "pacman";
    }
    // Check if `dnf` is available (Fedora)
    if (system("which dnf > /dev/null 2>&1") == 0) {
        return "dnf";
    }
    // Check if `yum` is available (Older Fedora/RHEL-based)
    if (system("which yum > /dev/null 2>&1") == 0) {
        return "yum";
    }
    // Check if `zypper` is available (openSUSE)
    if (system("which zypper > /dev/null 2>&1") == 0) {
        return "zypper";
    }
    // Check if `brew` is available (macOS or Linux with Homebrew)
    if (system("which brew > /dev/null 2>&1") == 0) {
        return "brew";
    }

    // If no known package manager is found
    return "unknown";
}

// Utility function to copy a file from src to dest, overwriting if necessary
void copy_file(const std::filesystem::path& src, const std::filesystem::path& dest) {
    try {
        // If the destination exists, remove it
        if (std::filesystem::exists(dest)) {
            std::cout << "Overriding existing file: " << dest << std::endl;
            std::filesystem::remove(dest);
        }

        // Copy the file
        std::filesystem::copy(src, dest);
        std::cout << "Copied: " << src << " -> " << dest << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error copying file: " << e.what() << std::endl;
    }
}

// Print a colored prompt message (yellow)
void prompt(const std::string& message) {
    std::cout << CYAN << message << RESET << std::endl;
}

// Print a colored error message (red)
void error(const std::string& message, bool exception = true) {
    std::cerr << RED << CROSS_MARK << " " << RESET << message << std::endl;
    if (exception) {
        throw std::runtime_error(message);
    }
}

// Print a colored success message (green)
void success(const std::string& message) {
    std::cout << GREEN << CHECK_MARK << " " << RESET << message << std::endl;
}

// Print an informational message (blue)
void info(const std::string& message) {
    std::cout << BLUE << INFORMATION << " " << RESET << message << std::endl;
}

std::string ask_input() {
    std::string input;
    std::cout << MAGENTA << "> " << RESET;
    std::getline(std::cin, input);
    return input;
}
vector<fs::path> select_configs(const fs::path& home) {
    vector<pair<string, fs::path>> common_configs = {
        {"Bash", home / ".bashrc"},
        {"Zsh", home / ".zshrc"},
        {"Neovim", home / ".config/nvim"},
        {"Kitty", home / ".config/kitty"},
        {"Git", home / ".gitconfig"},
        {"Tmux", home / ".tmux.conf"}
    };

    vector<fs::path> selected;

    prompt("Select configuration files to export:");

    for (const auto& [name, path] : common_configs) {
        cout << "➤ Include " << name << " config (" << path << ")? (y/n): ";
        char choice = ask_input()[0];
        if (choice == 'y' || choice == 'Y') {
            selected.push_back(path);
        }
    }

    prompt("Do you want to add custom paths? (y/n): ");
    char custom_choice = ask_input()[0];
    if (custom_choice == 'y' || custom_choice == 'Y') {
        while (true) {
            prompt("Enter full path (or type 'done' to finish): ");
            string input = ask_input();
            if (input == "done") break;

            fs::path custom_path = input;
            if (fs::exists(custom_path)) {
                selected.push_back(custom_path);
            } else {
                error("Path doesn't exist: " + input, false);
            }
        }
    }

    return selected;
}

// Function to collect package names from the user
void collect_package_names(ofstream& pkg_file) {
    prompt("Enter package names (type 'done' to finish):");

    std::string package_name;
    while (true) {
        package_name = ask_input();

        // Break if the user types 'done'
        if (package_name == "done") {
            break;
        }

        pkg_file << package_name << endl;
    }
}


string setup_package_manager() {
    prompt("Enter package manager (leave blank to auto detect): ");
    string package_manager = ask_input();
    if (package_manager.empty()) {
        package_manager = get_package_manager();
        success("Auto detected package manager: " + package_manager);
    } else {
        success("Using specified package manager: " + package_manager);
    }
    return package_manager;
}

#endif // UTILS_H
