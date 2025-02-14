#include <stdio.h>
#include <stdlib.h>
#include "archinstallcommands.h"
#include "stringformat.h"

#define BLOCK 20
#define MAX_USERS 5
#define MAX_PARTITIONS 5
#define MAX_SETTINGS 15
#define MAX_COMMANDS 25

struct User
{
    char name[BLOCK];
    char shell[BLOCK];
};
struct Partition
{
    char partition[BLOCK];
    char path[BLOCK];

    int is_swap;
    int bootable;
    int to_umount;
    int to_format;
    char format[BLOCK];
};

struct Setting
{
    char setting[BLOCK*5];
    char write_type[3];
    char destination[BLOCK*2];
};

struct Command
{
    char command[BLOCK*5];
};


int main() {
    struct User users[MAX_USERS];
    struct Partition partitions[MAX_PARTITIONS];
    /*struct Setting settings[MAX_SETTINGS];*/
    struct Command commands[MAX_COMMANDS];

    char *packages = (char*)malloc(BLOCK * sizeof(char));
    int vel = 0, velFull = BLOCK;
    /*char *packages;*/
    char buffer[BLOCK*5];
    int users_size=0, partitions_size=0, /*settings_size=0,*/ commands_size=0;
    int i;
    FILE *fp;
    /*get users*/
    fp=fopen("users.txt", "r");
    if(!fp) {
        printf("Failed to open users.txt. exiting.\n");
        return 32;
    }
    while (users_size<MAX_USERS && fgets(buffer,BLOCK*3,fp))
    {
        sscanf(buffer,"%20[^,],%s",users[users_size].name,users[users_size].shell);
        users_size++;
    }
    /*close users.txt*/
    fclose(fp);


    /*print users*/
    for(i=0;i<users_size;i++) {
        printf("%s:%s\n",users[i].name,users[i].shell);
    }

    /*get partitions*/
    fp=fopen("partitions.txt", "r");
    if(!fp) {
        printf("Failed to open partitions.txt. exiting.\n");
        return 50;
    }
    while (partitions_size<MAX_PARTITIONS && fgets(buffer,BLOCK*5,fp))
    {
        char *next=buffer;
        while(*next!=EOF && (*next==' ' || *next=='\n')) next++;
        if(*next=='#') continue;

        sscanf(buffer,"%20[^,],%20[^,],%d,%d,%d,%d,%20[^\n]",
            partitions[partitions_size].partition,
            partitions[partitions_size].path,
            &partitions[partitions_size].is_swap,
            &partitions[partitions_size].bootable,
            &partitions[partitions_size].to_umount,
            &partitions[partitions_size].to_format,
            partitions[partitions_size].format
        );
        partitions_size++;
    }
    /*close partitions.txt*/
    fclose(fp);

    /*print partitions*/
    for(i=0;i<partitions_size;i++) {
        printf("%d. |%s:%s-%d-%d-%d-%d:%s|\n", i+1,
            partitions[i].partition,
            partitions[i].path,
            partitions[i].is_swap,
            partitions[i].bootable,
            partitions[i].to_umount,
            partitions[i].to_format,
            partitions[i].format
        );
    }

    /*get packages*/
    fp=fopen("packages.txt", "r");
    if(!fp) {
        printf("Failed to open packages.txt. exiting.\n");
        return 70;
    }
    char c=fgetc(fp);
    while (c!=EOF)
    {
        while (c!=' ' && c!='\n' && c!=EOF)
        {
            packages[vel++]=c;
            if (vel >= velFull) {
                velFull += BLOCK;
                packages = (char *)realloc(packages, velFull * sizeof(char));
            }
            c=fgetc(fp);
        }
        while(c==' ' || c=='\n') {
            c=fgetc(fp);
        }
        packages[vel++]=' ';
    }
    packages[--vel]='\0';
    /*close packages.txt*/
    fclose(fp);

    /*print packages*/
    printf("|%s|\n",packages);

    /*get commands*/
    fp=fopen("commands.txt","r");
    if(!fp) {
        printf("Failed to open commands.txt. exiting.\n");
        return 187;
    }

    while(commands_size<MAX_COMMANDS) {
        int j=0;
        c = fgetc(fp);
        if(c==EOF) break;
        while(c!=EOF && c!='\n') {
            commands[commands_size].command[j++]=c;
            c=fgetc(fp);
        }
        commands[commands_size++].command[j]=0;
    }
    /*print commands*/
    for(i=0;i<commands_size;i++) {
        printf("command:%s|\n",commands[i].command);
    }
    /*close commands.txt*/
    fclose(fp);

    /*Create the build string*/
    int echo_msg_pos=0;
    /* First stage */
    /* Create the 1stage.sh script */
    fp = fopen("1stage.sh","w");

    if(!fp) {
        printf("Failed to create build.sh. exiting.\n");
        free(packages);
        return 149;
    }
    /* Put header */
    fputs(SCRIPT_HEADER,fp);
    fputs(TIMEDATECTL,fp);

    /* Partition */
    for(i=0;i<partitions_size;i++) {
        char *next = buffer;
        if(partitions[i].to_format) {
            strcpy(next, "mkfs.%s %s\n");
            fputs(sf(next,partitions[i].format, partitions[i].partition),fp);
        }
        if(partitions[i].is_swap) {
            strcpy(next, "mkswap %s\n");
            fputs(sf(next,partitions[i].partition),fp);
            strcpy(next, "swapon %s\n");
            fputs(sf(next,partitions[i].partition),fp);
            continue;
        }
        strcpy(next,"mount --mkdir %s %s\n");
        fputs(sf(next,partitions[i].partition,partitions[i].path),fp);

    }
    fputc('\n',fp);
    fputs(PACSTRAP,fp);
    fputs(GENFSTAB,fp);
    fputc('\n',fp);
    /* THIS PART IS UNSAFE!! TEST COMMANDS BEFORE DEPLOYING */
    fputs("umount -R /mnt\n\n",fp);
    for(i=0;i<partitions_size;i++) {
        char *next = buffer;
        if(!partitions[i].to_umount) {
            strcpy(next, "mount %s %s\n");
            fputs(sf(next,partitions[i].partition,partitions[i].path),fp);
        }
    }
    fputc('\n',fp);
    fputs(CPY2STAGE,fp);
    fputs(ECHOS[echo_msg_pos++],fp);
    fclose(fp);

    /* Second stage */
    fp = fopen("2stage.sh","w");

    if(!fp) {
        printf("Failed to create build.sh. exiting.\n");
        return 180;
    }
    fputs(SCRIPT_HEADER,fp);

    /* Put setup commands */
    for(i=0;i<commands_size;i++) {
        fputs(commands[i].command,fp);
        fputc('\n',fp);
    }

    /* Put pacman command and free packages */
    fputs("\npacman -S ",fp);
    fputs(packages,fp);
    fputc('\n',fp);
    free(packages);

    /* Setup grub */

    for(i=0;i<GRUBSETUP_COUNT;i++) {
        fputs(GRUBSETUP[i],fp);
    }

    /* Add users */
    fputc('\n',fp);
    strcpy(buffer,ECHOS[echo_msg_pos++]);
    int* l=(int*)malloc(sizeof(int));
    *l=0;
    for(i=0;i<users_size;i++) {
        *l+=strlen(users[i].name);
    }
    char *names = (char*) malloc(sizeof(char) * (*l+2));
    /*char* next = (char**) malloc(sizeof(char*));
    next=names;*/
    for(i=0;i<users_size;i++) {
        strcat(names,users[i].name);
        strcat(names," ");
    }
    *l=strlen(names);
    names[--*l]=0;
    /*name name... */
    fputs(sf(buffer,names),fp);
    free(l);
    free(names);
    /*free(next); */
    fputs("passwd",fp);
    fputc('\n',fp);

    for(i=0;i<users_size;i++) {
        char * next1 = buffer;
        strcpy(next1,USERADD);
        fputs(sf(next1,users[i].shell,users[i].name,users[i].name),fp);
    }


    /* echo rest of echo messages */
    for(i=echo_msg_pos;i<ECHO_COUNT;i++) {
        fputs(ECHOS[i],fp);
    }

    /* close all */
    fclose(fp);
    printf("Done.\n");



    return 0;
}

/*
Napravi strukturu koja sadezi sljedece informacije:

disk
mjesto gdje ce se uraditi mount
je li swap
je li boot
treba li se umountati nakon fstaba
treba li se formatirati
koji format da se koristi (sad realno samo ext4)
*/