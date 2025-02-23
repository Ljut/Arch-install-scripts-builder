# Arch Installer Scripts Builder (AISB)

## Introduction

This is a C program that creates two installation scripts for installing Arch Linux.

I created it as preparation for a programming exam.

## What Does the Program Do and What Are Its Limitations?

The program generates two shell scripts that you can run from the Arch Linux live installer to install Arch Linux. These scripts are named `1stage.sh` and `2stage.sh`, and it is important that they are in the same directory.

- `1stage.sh` is responsible for configuring partitions and running the `pacstrap` and `genfstab` commands. After that, it copies `2stage.sh` to `/mnt`, from where you will run it. Instructions on the screen will guide you.
- `2stage.sh` configures your machine using commands from `commands.txt`, installs packages from `packages.txt`, adds root and other users from `users.txt`, configures GRUB, and performs final preparations from `postinstall.sh`.

### Limitations
- The program assumes that the partition table is already set up.
- It is best used to replace an already installed Linux distribution, as this makes it easier to copy scripts to the installation medium.

## What Commands Should You Expect to Run While Executing the Scripts?

Here are the commands you should expect to run from the start of `1stage.sh` to the end, unless you have added additional steps that require user input. (I assume you are in the same directory as the scripts.):

```
sh 1stage.sh
y
arch-chroot /mnt
sh 2stage.sh
y
<your new root password>
<retype it>
<your new user password>
<retype it>
exit
umount -R /mnt
reboot
```

You will simply need to follow the on-screen instructions.

## About Configuration Files

The C program reads configurations from five different files and compiles them into two shell scripts (`1stage.sh` and `2stage.sh`).
These configuration files, in order of parsing, are:

- `users.txt`
- `partitions.txt`
- `packages.txt`
- `commands.txt`
- `postinstall.sh`

If any of these files are missing, the program will stop and will not generate the scripts.

### `users.txt`

This file contains a list of users that will be created during installation and is used in generating `2stage.sh`. It is formatted as a CSV (comma-separated values) file, where each row contains the following information:

`user_name,path_to_shell`

__File does not support comments.__

Example:
```csv
arch,/bin/bash
```

### `partitions.txt`

This file lists the partitions used during installation and where the system will be installed. It is used in generating `1stage.sh`.

It is formatted as a CSV (comma-separated values) file and supports comments using `#` at the start of a line. Each row contains the following information:

`disk,mount_point,is_swap,is_it_boot,should_umount_after_fstab?,should_format?,format_type`

Example:
```csv
# disk,mount_point,swap?,boot?,umount_after_fstab?,format?,format
/dev/nvme0n1p6,/mnt,0,0,0,1,ext4
# This is the boot partition for Windows, which will be unmounted
# after the genfstab command. It is included so that GRUB
# can detect Windows later.
/dev/nvme0n1p1,/mnt/boot,0,1,1,0,
/dev/nvme0n1p8,/mnt/home,0,0,0,0,
# This is the boot partition for Arch Linux.
/dev/nvme0n1p9,/mnt/boot,0,1,0,0,
/dev/nvme0n1p7,swap,1,0,0,0
```

It is important that all commas are included. If there is a trailing comma at the end of a line, it means the format type is unspecified. A value of `1` before the format type indicates formatting will occur, while `0` means it will not.

__NOTE:__ The program does not create partitions. The user must create them before running `1stage.sh` or `2stage.sh`.

### `packages.txt`

This file lists the packages to be installed. Package names can be separated by spaces or newlines (`\n`).

__File does not support comments.__

Example:
```
nano
sudo
networkmanager
openssh
git
```

__NOTE:__ If any package name is misspelled, the script will halt. It is recommended to manually execute the remaining commands from the script to avoid duplicate configurations.

__NOTE:__ Using newlines (`\n`) is recommended to prevent buffer overflow issues if a line is too long.

### `commands.txt`

This file contains commands to be executed at the beginning of `2stage.sh`.

Example:
```sh
ln -sf /usr/share/zoneinfo/Europe/City /etc/localtime
hwclock --systohc

echo "LANG=en_US.UTF-8" >> /etc/locale.conf
echo "KEYMAP=croat" >> /etc/vconsole.conf
echo "legion" > /etc/hostname
locale-gen
```

### `postinstall.sh`

This file contains commands to be executed after the installation, typically for configuring installed packages from `packages.txt`.

Example:

```sh
# Put here configurations and commands to run after base install
systemctl enable NetworkManager.service
```

__NOTE:__ `postinstall.sh` should not include `#!/bin/sh` or `set -e`, as these will already be included in `2stage.sh`.

---

