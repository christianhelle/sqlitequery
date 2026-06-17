# Development Container for SQLite Query Analyzer

This directory contains configuration for a development container that provides a consistent environment for building and testing the SQLite Query Analyzer.

## Features

- Full C++ development environment with GCC/Clang
- Qt 6.9.0 for GUI development
- PowerShell for cross-platform scripting
- SSH server for remote development
- Docker-in-Docker for building Snap packages

## Building Snap Packages

The development container is configured with Docker-in-Docker support for building Snap packages. You can use the `snap-build` command to build Snap packages from within the container:

```bash
# Navigate to your project directory
cd /workspaces/sqlitequery

# Build snap package
snap-build
```

The `snap-build` command is a wrapper around the Snapcraft Docker container that ensures:

1. Snap packages can be built within the development container
2. No need to install snapd or snapcraft directly in the container
3. Consistent build environment across all development machines

## Additional Notes

- The build command mounts your current directory into the Snapcraft container
- You can pass additional arguments to snapcraft by adding them to the `snap-build` command
- If you need to customize the snap building process further, you can modify the `.devcontainer/snap-build.sh` script