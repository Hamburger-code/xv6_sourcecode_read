#define T_DIR     1   // Directory
#define T_FILE    2   // File
#define T_DEVICE  3   // Device

struct stat {
  int dev;     // File system's disk device 文件系统的磁盘设备
  uint ino;    // Inode number  文件节点编号
  short type;  // Type of file //文件类型
  short nlink; // Number of links to file //文件链接数，或者说文件的别名有几个
  uint64 size; // Size of file in bytes //文件大小，有多少个字节
};
