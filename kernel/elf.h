#ifndef __ELF_H
#define __ELF_H
#include "stdint.h"

#define EI_NIDENT (16)
// elf header
struct elf_header {
  unsigned char e_ident[EI_NIDENT]; /* 魔术和一些info(大小端，版本) */
  uint16_t e_type;      /* 文件类型(可重定位文件，可执行文件) */
  uint16_t e_machine;   /* 该文件支持运行的硬件平台 */
  uint32_t e_version;   /* 版本信息 */
  uint32_t e_entry;     /* 加载后的虚拟地址 */
  uint32_t e_phoff;     /* 程序头表在文件中的偏移量*/
  uint32_t e_shoff;     /* 节头表在文件中的偏移量 */
  uint32_t e_flags;     /* 与处理器相关的标志 */
  uint16_t e_ehsize;    /* elf header 的字节大小 */
  uint16_t e_phentsize; /* 程序头表中每个条目(entry)的字节大小 */
  uint16_t e_phnum;     /* 程序头表中的条目数量 */
  uint16_t e_shentsize; /* 节头表中每个条目(entry)的字节大小 */
  uint16_t e_shnum;     /* 节头表中的条目数量 */
  uint16_t e_shstrndx;  /* string name table 在节头表中的索引 index */
};

struct program_header {
  uint32_t p_type;   /* 该段的类型(可加载，动态链接) */
  uint32_t p_offset; /* 本段在文件内的起始偏移地址 */
  uint32_t p_vaddr;  /* 指明本段在内存中的起始虚拟地址 */
  uint32_t p_paddr;  /* 物理地址 */
  uint32_t p_filesz; /* 该段在文件中的大小 */
  uint32_t p_memsz;  /* 该段在内存中的大小 */
  uint32_t p_flags;  /* 读写可执行等权限的相关标志位 */
  uint32_t p_align; /* 指明本段在文件和内存中的对齐方式(对齐或不对齐) */
};

#endif
