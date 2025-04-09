sudo apt-get update
sudo apt-get install -y python3 python3-pip
sudo apt install -y libglx-dev libgl1-mesa-dev libxkbcommon-x11-dev libfontconfig1 libdbus-1-dev
pip install aqtinstall --break-system-packages
aqt install-qt linux desktop 6.8.2
export QT_DIR=./6.8.2/gcc_64
export Qt6_DIR=./6.8.2/gcc_64
