sudo apt-get update
sudo apt-get install -y python3 python3-pip
sudo apt install -y libglx-dev libgl1-mesa-dev
pip install aqtinstall --break-system-packages
aqt install-qt linux desktop 6.8.2 --autodesktop