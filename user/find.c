#include "../kernel/types.h"
#include "../user/user.h"
#include "../kernel/fs.h"
#include "../kernel/stat.h"

// 判断文件名称是否等于指定名称
int nameEqualToName(char *path, char *name) {
    char *p;
    // 从路径的末尾向前查找，直到找到最后一个斜杠或者到达路径的开头。
    // 这样，p 将指向路径中最后一个斜杠后面的字符。
    for (p = path + strlen(path); p >= path && *p != '/'; p--);
    p++;
    return strcmp(p, name);
}

// find file by name
void find(char *path, char *find_name) {
    // init function
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // ls can not open
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // get stat
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
        case T_FILE:
            if (nameEqualToName(path, find_name) == 0) {
                printf("%s\n", path);
            }
            break;
        case T_DIR:
            printf("dir %d %s\n", st.type, path);
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0)
                    continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                // 防止递归..与.
                if (strcmp(buf, "./.") * strcmp(buf, "./..") != 0) {
                    printf("find: search %s\n", buf);
                    find(buf, find_name);
                }
            }
            break;
    }
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("argc should be equal to 3\n");
        exit(0);
    }
    find(argv[1], argv[2]);
    exit(0);
}