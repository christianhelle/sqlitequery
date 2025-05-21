#!/bin/bash
# This script provides an easy way to build snap packages in the development container

# Check if we're in the right directory
if [ ! -f "snapcraft.yaml" ] && [ ! -f "snap/snapcraft.yaml" ]; then
  echo "Error: No snapcraft.yaml found in current directory or snap/ subdirectory"
  echo "Please run this script from the project root directory"
  exit 1
fi

echo "Building snap package using snapcraft Docker container..."
docker pull snapcore/snapcraft:stable
docker run --rm -v "$(pwd)":/build -w /build snapcore/snapcraft:stable snapcraft "$@"

echo "Snap package build complete!"