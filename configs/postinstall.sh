# Put here configurations and commands to run after base install
systemctl enable NetworkManager.service
systemctl enable avahi-daemon.service
git clone https://aur.archlinux.org/yay-bin.git
cd yay-bin
makepkg -si
