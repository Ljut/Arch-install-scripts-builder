//
// Created by Zlatan Ljutika on 14. 2. 2025..
//

#ifndef ARCHINSTALLCOMMANDS_H
#define ARCHINSTALLCOMMANDS_H

char SCRIPT_HEADER[] = "#!/bin/sh\nset -e\n\n";
char TIMEDATECTL[] = "timedatectl\n";
char CPY2STAGE[] = "cp 2stage.sh /mnt/\n";

char PACSTRAP[] = "pacstrap -K /mnt base linux linux-firmware\n";
char GENFSTAB[] = "genfstab -U /mnt >> /mnt/etc/fstab\n";

#define GRUBSETUP_COUNT 3
char GRUBSETUP[GRUBSETUP_COUNT][100] = {
    "grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=GRUB\n",
    "echo \"GRUB_DISABLE_OS_PROBER=false\" >> /etc/default/grub",
    "grub-mkconfig -o /boot/grub/grub.cfg\n"
};

char USERADD[] = "useradd -m -G users,wheel,audio -s %s %s\npasswd %s\n\n";

#define ECHO_COUNT 6

char ECHOS[ECHO_COUNT][135] = {
    "echo \"Do: 'arch-chroot /mnt' and run 2stage.sh\"\n",
    "echo \"Run passwd to change root password and later for %s.\"\n",
    "echo \"By now you can exit chroot, unmount partitions and reboot system:\"\n",
    "echo \"exit\"\n",
    "echo \"umount -R /mnt\"\n",
    "echo \"reboot\"\n"
 };
#endif //ARCHINSTALLCOMMANDS_H
