#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

int checkargs(int argc, char *argv[])
{
    if (argc < 2) // is just one arg given?
    {
        fprintf(stderr, "usage %s \"suroutine\", \"filename\", \"text / mode\"\n", argv[0]);
        return -1;
    }
    else
    {
        // 3 arguments in argv
        if (strcmp(argv[1], "myopen") == 0 || strcmp(argv[1], "mylseek") == 0 || strcmp(argv[1], "mystat") == 0 || strcmp(argv[1], "myaccess") == 0 || strcmp(argv[1], "myreaddir") == 0)
        {
            // are 3 of 3 expected arguments given?
            if (argc < 3)
            {
                fprintf(stderr, "usage %s \"%s\", \"filename\", \"\"\n", argv[0], argv[1]);
                return -1;
            }
            // 3 of 3 arguments given
            else
            {
                return 1;
            }
        }
        // 4 arguments in argv
        else if (strcmp(argv[1], "mycreat") == 0)
        {
            // are 4 of 4 expected arguments given? last argument "text"
            if (argc < 4)
            {
                fprintf(stderr, "usage %s \"%s\", \"filename\", \"text\"\n", argv[0], argv[1]);
                return -1;
            }
            // 4 of 4 arguments given
            else
            {
                return 1;
            }
        }
        // 4 arguments in argv
        else if (strcmp(argv[1], "mychmod") == 0 || strcmp(argv[1], "mymkdir") == 0)
        {
            // are 4 of 4 expected arguments given? last argument "mode"
            if (argc < 4)
            {
                fprintf(stderr, "usage %s \"%s\", \"filename\", \"mode\"\n", argv[0], argv[1]);
                return -1;
            }
            // 4 of 4 arguments given
            else
            {
                return 1;
            }
        }
        else
        {
            printf("Argv: %s\n", argv[1]);
            return -1;
        }
        fprintf(stderr, "error");
    }
}

#pragma region actions

void performCreate(char *filepath[], char *text[])
{
    int fd;
    if ((fd = creat(*filepath, S_IRUSR | S_IWUSR)) < 0)
    {
        perror("mycreat error");
        exit(0);
    }
    else
    {
        printf("successfully created\n");
        write(fd, *text, strlen(*text));
        close(fd);
    }
}

void performOpen(char *filepath[])
{
    int fd;
    char readBuf[1024];
    if ((fd = open(*filepath, S_IRUSR | S_IWUSR)) < 0)
    {
        perror("myopen error");
        exit(0);
    }
    else
    {
        char c;
        int readd = read(fd, readBuf, 1024);
        printf("%i characters read, text:\n%s\n", readd, readBuf);
        close(fd);
    }
}
void performLseek(char *filepath[])
{
    int fd;
    char readBuf[1024];
    if ((fd = open(*filepath, S_IRUSR | S_IWUSR | O_SYNC | O_RDWR)) < 0)
    {
        perror("myopen error");
        exit(0);
    }
    else
    {
        char text[] = "new text, created by lseek";
        //printf("%s\n", text);
        int a = lseek(fd, 0, SEEK_END);
        printf("till end: %d\n%d\n", a, fd);
        lseek(fd, 0, SEEK_END);
        //perror("error1");
        int i = write(fd, &text, strlen(text));
        printf("wrote: %d\n%d\n", i, fd);
        perror("error2");

        //reset files cursor
        lseek(fd, 0, SEEK_SET);
        char c;
        int readd = read(fd, readBuf, 1024);
        printf("%i characters read, text:\n%s\n", readd, readBuf);
        close(fd);
    }
}
void performFstat(char *filepath[])
{
    printf("in mystat");
    int fd;
    if ((fd = open(*filepath, S_IRUSR | S_IWUSR | O_RDONLY)) < 0)
    {
        perror("myFstat error");
        exit(0);
    }
    else
    {
        struct stat fileStat;
        if (fstat(fd, &fileStat) < 0)
        {
            perror("myFstat error");
            exit(0);
        }
        else
        {
            printf("Information for %s\n", *filepath);
            printf("---------------------------\n");
            printf("File Size: \t\t%ld bytes\n", fileStat.st_size);
            printf("Number of Links: \t%ld\n", fileStat.st_nlink);
            printf("File inode: \t\t%ld\n", fileStat.st_ino);

            printf("File Permissions: \t");
            printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
            printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
            printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
            printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
            printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
            printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
            printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
            printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
            printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
            printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
            printf("\n\nTime in 12h format!!!\n");

            // Last access
            printf ( "Last access:                  %s", ctime(&fileStat.st_atim.tv_sec));
            // Last change of permissions
            printf ( "Last change of permissions:   %s", ctime(&fileStat.st_ctim));
            // Last modification of content
            printf ( "Last modification of content: %s", ctime(&fileStat.st_mtim));
        }
    }
}
#pragma endregion

void selectAndPerformAction(int argc, char *argv[])
{
    if (checkargs(argc, argv))
    {
        if (strcmp(argv[1], "mycreat") == 0)
        {
            performCreate(&argv[2], &argv[3]);
        }
        else if (strcmp(argv[1], "myopen") == 0)
        {
            performOpen(&argv[2]);
        }
        else if (strcmp(argv[1], "mylseek") == 0)
        {
            performLseek(&argv[2]);
        }
        else if (strcmp(argv[1], "mystat") == 0)
        {
            performFstat(&argv[2]);
        }
        else
        {
            printf("Not implemented yet!");
        }
    }
}

int main(int argc, char *argv[])
{
    selectAndPerformAction(argc, argv);
}