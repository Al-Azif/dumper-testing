#ifndef PFS_HPP_
#define PFS_HPP_

#include <cstdint>
#include <string>
#include <vector>

#define PFS_MAGIC 0x0B2A330100000000

#define PFS_DUMP_BUFFER 0x4000

namespace pfs {
typedef struct {
  uint64_t version;
  uint64_t magic;
  uint32_t id[2];
  char fmode;
  char clean;
  char ronly;
  char rsv;
  uint16_t mode;
  uint16_t unk1;
  uint32_t blocksz;
  uint32_t nbackup;
  uint64_t nblock;
  uint64_t ndinode;
  uint64_t ndblock;
  uint64_t ndinodeblock;
  uint64_t superroot_ino;
} pfs_header;

typedef struct {
  uint16_t mode;
  uint16_t nlink;
  uint32_t flags;
  uint64_t size;
  uint64_t size_compressed;
  uint64_t unix_time[4];
  uint32_t time_nsec[4];
  uint32_t uid;
  uint32_t gid;
  uint64_t spare[2];
  uint32_t blocks;
  uint32_t db[12];
  uint32_t ib[5];
} di_d32;

typedef struct {
  uint32_t ino;
  uint32_t type;
  uint32_t namelen;
  uint32_t entsize;
} dirent_t;

extern pfs_header header;
extern size_t pfs_size;
extern size_t pfs_copied;
extern std::vector<di_d32> inodes;

extern std::ifstream pfs_input;

void __parse_directory(uint32_t ino, uint32_t level, const std::string &output_path, bool calculate_only);
void calculate_pfs_size(uint32_t ino, uint32_t level);
void dump_pfs(uint32_t ino, uint32_t level, const std::string &output_path);
void extract(const std::string &pfs_path, const std::string &output_path);
} // namespace pfs

#endif // PFS_HPP_
