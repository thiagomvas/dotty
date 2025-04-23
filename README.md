# Dotty

Dotty is a personal configuration management and setup tool for quickly configuring a new system. It installs packages and creates symlinks for configuration files.

## Features
- **Package Manager Detection**: Supports `apt`, `pacman`, `brew`, and others.
- **Symlink Creation**: Creates symlinks for configuration files from a central `configRoot`.
- **Test Mode**: Run setup without making any actual changes (`--test`).

## Setup

1. Clone the repository:
   ```bash
   git clone https://github.com/thiagomvas/dotty
   cd dotty
   ```

2. Make the setup script executable:
   ```bash
   chmod +x setup_bootstrap.sh
   ```

4. Run the bootstrap script:
   ```bash
   ./setup_bootstrap.sh
   ```

## Applying the dotfiles
1. Make the installation script executable:
   ```bash
   chmod +x setup_bootstrap.sh
   ```
3. Run the bootstrap script
   ```bash
   ./installation_bootstrap.sh
   ```
5. To run in test mode (no changes made):
   ```bash
   ./installation_bootstrap.sh --test
   ```

## License
MIT License.
