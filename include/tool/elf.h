#pragma once

#include <type.h>

/* Type for a 16-bit quantity.  */
typedef u16 Elf32_Half;
typedef u16 Elf64_Half;

/* Types for signed and unsigned 32-bit quantities.  */
typedef u32 Elf32_Word;
typedef	i32 Elf32_Sword;
typedef u32 Elf64_Word;
typedef	i32 Elf64_Sword;

/* Types for signed and unsigned 64-bit quantities.  */
typedef u64 Elf32_Xword;
typedef	i64 Elf32_Sxword;
typedef u64 Elf64_Xword;
typedef	i64 Elf64_Sxword;

/* Type of addresses.  */
typedef u32 Elf32_Addr;
typedef u64 Elf64_Addr;

/* Type of file offsets.  */
typedef u32 Elf32_Off;
typedef u64 Elf64_Off;

/* Type for section indices, which are 16-bit quantities.  */
typedef u16 Elf32_Section;
typedef u16 Elf64_Section;

/* Type for version symbol information.  */
typedef Elf32_Half Elf32_Versym;
typedef Elf64_Half Elf64_Versym;

/* The ELF file header.  This appears at the start of every ELF file.  */

#define EI_NIDENT (16)

typedef struct
{
  unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
  Elf64_Half	e_type;			/* Object file type */
  Elf64_Half	e_machine;		/* Architecture */
  Elf64_Word	e_version;		/* Object file version */
  Elf64_Addr	e_entry;		/* Entry point virtual address */
  Elf64_Off	    e_phoff;		/* Program header table file offset */
  Elf64_Off	    e_shoff;		/* Section header table file offset */
  Elf64_Word	e_flags;		/* Processor-specific flags */
  Elf64_Half	e_ehsize;		/* ELF header size in bytes */
  Elf64_Half	e_phentsize;    /* Program header table entry size */
  Elf64_Half	e_phnum;		/* Program header table entry count */
  Elf64_Half	e_shentsize;	/* Section header table entry size */
  Elf64_Half	e_shnum;		/* Section header table entry count */
  Elf64_Half	e_shstrndx;		/* Section header string table index */
} Elf64_Ehdr;

typedef struct
{
  Elf64_Word	p_type;			/* Segment type */
  Elf64_Word	p_flags;		/* Segment flags */
  Elf64_Off	    p_offset;		/* Segment file offset */
  Elf64_Addr	p_vaddr;		/* Segment virtual address */
  Elf64_Addr	p_paddr;		/* Segment physical address */
  Elf64_Xword	p_filesz;		/* Segment size in file */
  Elf64_Xword	p_memsz;		/* Segment size in memory */
  Elf64_Xword	p_align;		/* Segment alignment */
} Elf64_Phdr;

#define PT_LOAD 1