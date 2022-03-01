/*
 * =====================================================================================
 *
 *       Filename:  eelf.h
 *
 *    Description:  ELF format type definition.
 *
 *        Version:  Melis3.0
 *         Create:  2018-03-05 16:52:55
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2018-03-05 16:53:13
 *
 * =====================================================================================
 */

#ifndef _EELF_H_
#define _EELF_H_

//定义elf基本数据类型
typedef unsigned short int      Elf32_Half;         // Type for a 16-bit quantity.
typedef unsigned int            Elf32_Word;         // Types for signed and unsigned 32-bit quantities.
typedef int                     Elf32_Sword;
typedef unsigned int            Elf32_Addr;         // Type of addresses.
typedef unsigned int            Elf32_Off;          // Type of file offsets.
typedef unsigned short int      Elf32_Section;      // Type for section indices, which are 16-bit quantities.
typedef unsigned int            Elf32_Symndx;       // Type of symbol indices.
//定义elf文件头
#define EI_NIDENT 16
typedef struct __ELF32HEAD
{
    unsigned char       ident[EI_NIDENT];           // Magic number and other info
    Elf32_Half          type;                       // Object file type
    Elf32_Half          machine;                    // Architecture
    Elf32_Word          version;                    // Object file version
    Elf32_Addr          entry;                      // Entry point virtual address
    Elf32_Off           phoff;                      // Program header table file offset
    Elf32_Off           shoff;                      // Section header table file offset
    Elf32_Word          flags;                      // Processor-specific flags
    Elf32_Half          ehsize;                     // ELF header size in bytes
    Elf32_Half          phentsize;                  // Program header table entry size
    Elf32_Half          phnum;                      // Program header table entry count
    Elf32_Half          shentsize;                  // Section header table entry size
    Elf32_Half          shnum;                      // Section header table entry count
    Elf32_Half          shstrndx;                   // Section header string table index
} __elf32_head_t;

/* Fields in the e_ident array.  The EI_* macros are indices into the
 * array.  The macros under each EI_* macro are the values the byte may have.
 */
#define     EI_MAG0                         0                       // File identification byte 0 index  
#define     ELFMAG0                         0x7f                    // Magic number byte 0  
#define     EI_MAG1                         1                       // File identification byte 1 index  
#define     ELFMAG1                         'E'                     // Magic number byte 1  
#define     EI_MAG2                         2                       // File identification byte 2 index  
#define     ELFMAG2                         'L'                     // Magic number byte 2  
#define     EI_MAG3                         3                       // File identification byte 3 index  
#define     ELFMAG3                         'F'                     // Magic number byte 3  

// Conglomeration of the identification bytes, for easy testing as a word.
#define     ELFMAG                          "\177ELF"
#define     SELFMAG                         4
#define     EI_CLASS                        4                       // File class byte index  
#define     ELFCLASSNONE                    0                       // Invalid class  
#define     ELFCLASS32                      1                       // 32-bit objects  
#define     ELFCLASS64                      2                       // 64-bit objects  
#define     ELFCLASSNUM                     3

#define     EI_DATA                         5                       // Data encoding byte index  
#define     ELFDATANONE                     0                       // Invalid data encoding  
#define     ELFDATA2LSB                     1                       // 2's complement, little endian  
#define     ELFDATA2MSB                     2                       // 2's complement, big endian  
#define     ELFDATANUM                      3

#define     EI_VERSION                      6                       // File version byte index  
// Value must be EV_CURRENT
#define     EI_OSABI                        7                       // OS ABI identification  
#define     ELFOSABI_SYSV                   0                       // UNIX System V ABI  
#define     ELFOSABI_HPUX                   1                       // HP-UX  
#define     ELFOSABI_ARM                    97                      // ARM  
#define     ELFOSABI_STANDALONE             255                     // Standalone (embedded) application  
#define     EI_ABIVERSION                   8                       // ABI version  
#define     EI_PAD                          9                       // Byte index of padding bytes  

// Legal values for e_type (object file type).
#define     ET_NONE                         0                       // No file type  
#define     ET_REL                          1                       // Relocatable file  
#define     ET_EXEC                         2                       // Executable file  
#define     ET_DYN                          3                       // Shared object file  
#define     ET_CORE                         4                       // Core file  
#define     ET_NUM                          5                       // Number of defined types  
#define     ET_LOPROC                       0xff00                  // Processor-specific  
#define     ET_HIPROC                       0xffff                  // Processor-specific  

// Legal values for e_machine (architecture).
#define     EM_NONE                         0                       // No machine  
#define     EM_M32                          1                       // AT&amp;T WE 32100  
#define     EM_SPARC                        2                       // SUN SPARC  
#define     EM_386                          3                       // Intel 80386  
#define     EM_68K                          4                       // Motorola m68k family  
#define     EM_88K                          5                       // Motorola m88k family  
#define     EM_486                          6                       // Intel 80486  
#define     EM_860                          7                       // Intel 80860  
#define     EM_MIPS                         8                       // MIPS R3000 big-endian  
#define     EM_S370                         9                       // Amdahl  
#define     EM_MIPS_RS4_BE                  10                      // MIPS R4000 big-endian  
#define     EM_ARM                          40                      // ARM  
#define     EM_MIPS_X                       51                      // Stanford MIPS-X  
#define     EM_NUM                          54

// Legal values for e_version (version).
#define     EV_NONE                         0                       // Invalid ELF version  
#define     EV_CURRENT                      1                       // Current version  
#define     EV_NUM                          2

// Section header.
typedef struct __ELF32SHEAD
{
    Elf32_Word          name;                                       // Section name (string tbl index)
    Elf32_Word          type;                                       // Section type
    Elf32_Word          flags;                                      // Section flags
    Elf32_Addr          addr;                                       // Section virtual addr at execution
    Elf32_Off           offset;                                     // Section file offset
    Elf32_Word          size;                                       // Section size in bytes
    Elf32_Word          link;                                       // Link to another section
    Elf32_Word          info;                                       // Additional section information
    Elf32_Word          addralign;                                  // Section alignment
    Elf32_Word          entsize;                                    // Entry size if section holds table
} __elf32_shead_t;

// Special section indices.
#define     SHN_UNDEF                        0                       // Undefined section  
#define     SHN_LORESERVE                    0xff00                  // Start of reserved indices  
#define     SHN_LOPROC                       0xff00                  // Start of processor-specific  
#define     SHN_HIPROC                       0xff1f                  // End of processor-specific  
#define     SHN_ABS                          0xfff1                  // Associated symbol is absolute  
#define     SHN_COMMON                       0xfff2                  // Associated symbol is common  
#define     SHN_HIRESERVE                    0xffff                  // End of reserved indices  

// Legal values for sh_type (section type).
#define     EELF_SHT_NULL                   0                   // Section header table entry unused  
#define     EELF_SHT_PROGBITS               1                   // Program data  
#define     EELF_SHT_SYMTAB                 2                   // Symbol table  
#define     EELF_SHT_STRTAB                 3                   // String table  
#define     EELF_SHT_RELA                   4                   // Relocation entries with addends  
#define     EELF_SHT_HASH                   5                   // Symbol hash table  
#define     EELF_SHT_DYNAMIC                6                   // Dynamic linking information  
#define     EELF_SHT_NOTE                   7                   // Notes  
#define     EELF_SHT_NOBITS                 8                   // Program space with no data (bss)  
#define     EELF_SHT_REL                    9                   // Relocation entries, no addends  
#define     EELF_SHT_SHLIB                  10                  // Reserved  
#define     EELF_SHT_DYNSYM                 11                  // Dynamic linker symbol table  
#define     EELF_SHT_NUM                    12                      // Number of defined types.   
#define     EELF_SHT_LOOS                   0x60000000          // Start OS-specific  
#define     EELF_SHT_LOSUNW                 0x6ffffffb          // Sun-specific low bound.   
#define     EELF_SHT_SUNW_COMDAT            0x6ffffffb
#define     EELF_SHT_SUNW_syminfo           0x6ffffffc
#define     EELF_SHT_GNU_verdef             0x6ffffffd          // Version definition section.   
#define     EELF_SHT_GNU_verneed            0x6ffffffe          // Version needs section.   
#define     EELF_SHT_GNU_versym             0x6fffffff          // Version symbol table.   
#define     EELF_SHT_HISUNW                 0x6fffffff          // Sun-specific high bound.   
#define     EELF_SHT_HIOS                   0x6fffffff          // End OS-specific type  
#define     EELF_SHT_LOPROC                 0x70000000          // Start of processor-specific  
#define     EELF_SHT_HIPROC                 0x7fffffff          // End of processor-specific  
#define     EELF_SHT_LOUSER                 0x80000000          // Start of application-specific  
#define     EELF_SHT_HIUSER                 0x8fffffff          // End of application-specific  

// Legal values for sh_flags (section flags).
#define     EELF_SHF_WRITE                  (1 << 0)            // Writable  
#define     EELF_SHF_ALLOC                  (1 << 1)            // Occupies memory during execution  
#define     EELF_SHF_EXECINSTR              (1 << 2)            // Executable  
#define     EELF_SHF_MASKPROC               0xf0000000          // Processor-specific  

// Symbol table entry.
typedef struct
{
    Elf32_Word          st_name;                // Symbol name (string tbl index)
    Elf32_Addr          st_value;               // Symbol value
    Elf32_Word          st_size;                // Symbol size
    unsigned char       st_info;                // Symbol type and binding
    unsigned char       st_other;               // No defined meaning, 0
    Elf32_Section       st_shndx;               // Section index
} Elf32_Sym;


// The syminfo section if available contains additional information about
// every dynamic symbol.
typedef struct
{
    Elf32_Half              si_boundto;         // Direct bindings, symbol bound to
    Elf32_Half              si_flags;           // Per symbol flags
} Elf32_Syminfo;

// Possible values for si_boundto.
#define SYMINFO_BT_SELF                 0xffff  // Symbol bound to self  
#define SYMINFO_BT_PARENT               0xfffe  // Symbol bound to parent  
#define SYMINFO_BT_LOWRESERVE           0xff00  // Beginning of reserved entries  

// Possible bitmasks for si_flags.
#define SYMINFO_FLG_DIRECT              0x0001  // Direct bound symbol  
#define SYMINFO_FLG_PASSTHRU            0x0002  // Pass-thru symbol for translator  
#define SYMINFO_FLG_COPY                0x0004  // Symbol is a copy-reloc  
#define SYMINFO_FLG_LAZYLOAD            0x0008  // Symbol bound to object to be lazy loaded  
// Syminfo version values.
#define SYMINFO_NONE                    0
#define SYMINFO_CURRENT                 1
#define SYMINFO_NUM                     2


// Special section index.
#define SHN_UNDEF       0               // No section, undefined symbol.   

// How to extract and insert information held in the st_info field.
#define ELF32_ST_BIND(val)                      (((unsigned char) (val)) >> 4)
#define ELF32_ST_TYPE(val)                      ((val) & 0xf)
#define ELF32_ST_INFO(bind, type)       (((bind) << 4) + ((type) & 0xf))

// Legal values for ST_BIND subfield of st_info (symbol binding).
#define STB_LOCAL       0               // Local symbol  
#define STB_GLOBAL      1               // Global symbol  
#define STB_WEAK        2               // Weak symbol  
#define STB_NUM         3               // Number of defined types.   
#define STB_LOOS        10              // Start of OS-specific  
#define STB_HIOS        12              // End of OS-specific  
#define STB_LOPROC      13              // Start of processor-specific  
#define STB_HIPROC      15              // End of processor-specific  

// Legal values for ST_TYPE subfield of st_info (symbol type).
#define STT_NOTYPE      0               // Symbol type is unspecified  
#define STT_OBJECT      1               // Symbol is a data object  
#define STT_FUNC        2               // Symbol is a code object  
#define STT_SECTION     3               // Symbol associated with a section  
#define STT_FILE        4               // Symbol's name is file name  
#define STT_NUM         5               // Number of defined types.   
#define STT_LOOS        11              // Start of OS-specific  
#define STT_HIOS        12              // End of OS-specific  
#define STT_LOPROC      13              // Start of processor-specific  
#define STT_HIPROC      15              // End of processor-specific  

// Symbol table indices are found in the hash buckets and chain table
// of a symbol hash table section.  This special index value indicates
// the end of a chain, meaning no further symbols are found in that bucket.
// End of a chain.
#define STN_UNDEF       0

// Relocation table entry without addend (in section of type SHT_REL).
typedef struct
{
    Elf32_Addr  r_offset;               // Address
    Elf32_Word  r_info;                 // Relocation type and symbol index
} Elf32_Rel;

// I have seen two different definitions of the Elf64_Rel and
// Elf64_Rela structures, so we'll leave them out until Novell (or
// whoever) gets their act together.
// The following, at least, is used on Sparc v9, MIPS, and Alpha.

// Relocation table entry with addend (in section of type SHT_RELA).
typedef struct
{
    Elf32_Addr  r_offset;               // Address
    Elf32_Word  r_info;                 // Relocation type and symbol index
    Elf32_Sword r_addend;               // Addend
} Elf32_Rela;

// How to extract and insert information held in the r_info field.
#define ELF32_R_SYM(val)                ((val) >> 8)
#define ELF32_R_TYPE(val)               ((val) & 0xff)
#define ELF32_R_INFO(sym, type) (((sym) << 8) + ((type) & 0xff))

// Program segment header.
typedef struct
{
    Elf32_Word p_type;                 // Segment type
    Elf32_Off          p_offset;               // Segment file offset
    Elf32_Addr p_vaddr;                // Segment virtual address
    Elf32_Addr p_paddr;                // Segment physical address
    Elf32_Word p_filesz;               // Segment size in file
    Elf32_Word p_memsz;                // Segment size in memory
    Elf32_Word p_flags;                // Segment flags
    Elf32_Word p_align;                // Segment alignment
} Elf32_Phdr;

// Legal values for p_type (segment type).
#define PT_NULL         0                       // Program header table entry unused  
#define PT_LOAD         1                       // Loadable program segment  
#define PT_DYNAMIC      2                       // Dynamic linking information  
#define PT_INTERP       3                       // Program interpreter  
#define PT_NOTE         4                       // Auxiliary information  
#define PT_SHLIB        5                       // Reserved  
#define PT_PHDR         6                       // Entry for header table itself  
#define PT_NUM          7                       // Number of defined types.   
#define PT_LOOS         0x60000000      // Start of OS-specific  
#define PT_HIOS         0x6fffffff      // End of OS-specific  
#define PT_LOPROC       0x70000000      // Start of processor-specific  
#define PT_HIPROC       0x7fffffff      // End of processor-specific  

// Legal values for p_flags (segment flags).
#define PF_X            (1 << 0)        // Segment is executable  
#define PF_W            (1 << 1)        // Segment is writable  
#define PF_R            (1 << 2)        // Segment is readable  
#define PF_MASKPROC     0xf0000000      // Processor-specific  

// Legal values for note segment descriptor types for core files.
#define NT_PRSTATUS     1                       // Contains copy of prstatus struct  
#define NT_FPREGSET     2                       // Contains copy of fpregset struct  
#define NT_PRPSINFO     3                       // Contains copy of prpsinfo struct  
#define NT_PRXREG       4                       // Contains copy of prxregset struct  
#define NT_PLATFORM     5                       // String from sysinfo(SI_PLATFORM)  
#define NT_AUXV         6                       // Contains copy of auxv array  
#define NT_GWINDOWS     7                       // Contains copy of gwindows struct  
#define NT_PSTATUS      10                      // Contains copy of pstatus struct  
#define NT_PSINFO       13                      // Contains copy of psinfo struct  
#define NT_PRCRED       14                      // Contains copy of prcred struct  
#define NT_UTSNAME      15                      // Contains copy of utsname struct  
#define NT_LWPSTATUS    16              // Contains copy of lwpstatus struct  
#define NT_LWPSINFO     17                      // Contains copy of lwpinfo struct  

// Legal values for the  note segment descriptor types for object files.
#define NT_VERSION      1                       // Contains a version string.   

// Dynamic section entry.
typedef struct
{
    Elf32_Sword d_tag;                  // Dynamic entry type
    union
    {
        Elf32_Word d_val;                 // Integer value
        Elf32_Addr d_ptr;                 // Address value
    } d_un;
} Elf32_Dyn;

// Legal values for d_tag (dynamic entry type).
#define DT_NULL         0               // Marks end of dynamic section  
#define DT_NEEDED       1               // Name of needed library  
#define DT_PLTRELSZ     2               // Size in bytes of PLT relocs  
#define DT_PLTGOT       3               // Processor defined value  
#define DT_HASH         4               // Address of symbol hash table  
#define DT_STRTAB       5               // Address of string table  
#define DT_SYMTAB       6               // Address of symbol table  
#define DT_RELA         7               // Address of Rela relocs  
#define DT_RELASZ       8               // Total size of Rela relocs  
#define DT_RELAENT      9               // Size of one Rela reloc  
#define DT_STRSZ        10              // Size of string table  
#define DT_SYMENT       11              // Size of one symbol table entry  
#define DT_INIT         12              // Address of init function  
#define DT_FINI         13              // Address of termination function  
#define DT_SONAME       14              // Name of shared object  
#define DT_RPATH        15              // Library search path  
#define DT_SYMBOLIC     16              // Start symbol search here  
#define DT_REL          17              // Address of Rel relocs  
#define DT_RELSZ        18              // Total size of Rel relocs  
#define DT_RELENT       19              // Size of one Rel reloc  
#define DT_PLTREL       20              // Type of reloc in PLT  
#define DT_DEBUG        21              // For debugging; unspecified  
#define DT_TEXTREL      22              // Reloc might modify .text  
#define DT_JMPREL       23              // Address of PLT relocs  
#define DT_BIND_NOW     24              // Process relocations of object  
#define DT_INIT_ARRAY   25              // Array with addresses of init fct  
#define DT_FINI_ARRAY   26              // Array with addresses of fini fct  
#define DT_INIT_ARRAYSZ 27              // Size in bytes of DT_INIT_ARRAY  
#define DT_FINI_ARRAYSZ 28              // Size in bytes of DT_FINI_ARRAY  
#define DT_NUM          29              // Number used  
#define DT_LOOS         0x60000000      // Start of OS-specific  
#define DT_HIOS         0x6fffffff      // End of OS-specific  
#define DT_LOPROC       0x70000000      // Start of processor-specific  
#define DT_HIPROC       0x7fffffff      // End of processor-specific  
#define DT_PROCNUM      DT_MIPS_NUM     // Most used by any processor  

// DT_* entries which fall between DT_VALRNGHI &amp; DT_VALRNGLO use the
// Dyn.d_un.d_val field of the Elf*_Dyn structure.  This follows Sun's approach.
#define DT_VALRNGLO     0x6ffffd00
#define DT_POSFLAG_1    0x6ffffdfd      // Flags for DT_* entries, effecting
//the following DT_* entry.
#define DT_SYMINSZ      0x6ffffdfe      // Size of syminfo table (in bytes)  
#define DT_SYMINENT     0x6ffffdff      // Entry size of syminfo  
#define DT_VALRNGHI     0x6ffffdff

// DT_* entries which fall between DT_ADDRRNGHI &amp; DT_ADDRRNGLO use the
// Dyn.d_un.d_ptr field of the Elf*_Dyn structure.

//   If any adjustment is made to the ELF object after it has been
//   built these entries will need to be adjusted.
#define DT_ADDRRNGLO    0x6ffffe00
#define DT_SYMINFO      0x6ffffeff      // syminfo table  
#define DT_ADDRRNGHI    0x6ffffeff

// The versioning entry types.  The next are defined as part of the GNU extension.
#define DT_VERSYM       0x6ffffff0

// These were chosen by Sun.
#define DT_FLAGS_1      0x6ffffffb      // State flags, see DF_1_* below.   
#define DT_VERDEF       0x6ffffffc      // Address of version definition  table  
#define DT_VERDEFNUM    0x6ffffffd      // Number of version definitions  
#define DT_VERNEED      0x6ffffffe      // Address of table with needed  versions  
#define DT_VERNEEDNUM   0x6fffffff      // Number of needed versions  
#define DT_VERSIONTAGIDX(tag)   (DT_VERNEEDNUM - (tag)) // Reverse order!  
#define DT_VERSIONTAGNUM 16

// Sun added these machine-independent extensions in the &quot;processor-specific&quot;
//   range.  Be compatible.
#define DT_AUXILIARY    0x7ffffffd      // Shared object to load before self 
#define DT_FILTER       0x7fffffff      // Shared object to get values from 
#define DT_EXTRATAGIDX(tag)     ((Elf32_Word)-((Elf32_Sword) (tag) <<1>>1)-1)
#define DT_EXTRANUM     3

// State flags selectable in the `d_un.d_val' element of the DT_FLAGS_1
// entry in the dynamic section.
#define DF_1_NOW                0x00000001      // Set RTLD_NOW for this object.   
#define DF_1_GLOBAL             0x00000002      // Set RTLD_GLOBAL for this object.   
#define DF_1_GROUP              0x00000004      // Set RTLD_GROUP for this object.   
#define DF_1_NODELETE           0x00000008      // Set RTLD_NODELETE for this object. 
#define DF_1_LOADFLTR           0x00000010      // Trigger filtee loading at runtime. 
#define DF_1_INITFIRST          0x00000020      // Set RTLD_INITFIRST for this object 
#define DF_1_NOOPEN             0x00000040      // Set RTLD_NOOPEN for this object.   

// Version definition sections.
typedef struct
{
    Elf32_Half  vd_version;             // Version revision
    Elf32_Half  vd_flags;               // Version information
    Elf32_Half  vd_ndx;                 // Version Index
    Elf32_Half  vd_cnt;                 // Number of associated aux entries
    Elf32_Word  vd_hash;                // Version name hash value
    Elf32_Word  vd_aux;                 // Offset in bytes to verdaux array
    Elf32_Word  vd_next;                // Offset in bytes to next verdef  entry
} Elf32_Verdef;

// Legal values for vd_version (version revision).
#define VER_DEF_NONE    0               // No version  
#define VER_DEF_CURRENT 1               // Current version  
#define VER_DEF_NUM     2                       // Given version number  

// Legal values for vd_flags (version information flags).
#define VER_FLG_BASE    0x1             // Version definition of file itself  
#define VER_FLG_WEAK    0x2             // Weak version identifier  

// Auxialiary version information.
typedef struct
{
    Elf32_Word  vda_name;               // Version or dependency names
    Elf32_Word  vda_next;               // Offset in bytes to next verdaux  entry
} Elf32_Verdaux;

// Version dependency section.
typedef struct
{
    Elf32_Half  vn_version;             // Version of structure
    Elf32_Half  vn_cnt;                 // Number of associated aux entries
    Elf32_Word  vn_file;                // Offset of filename for this  dependency
    Elf32_Word  vn_aux;                 // Offset in bytes to vernaux array
    Elf32_Word  vn_next;                // Offset in bytes to next verneed entry
} Elf32_Verneed;

// Legal values for vn_version (version revision).
#define VER_NEED_NONE    0              // No version  
#define VER_NEED_CURRENT 1              // Current version  
#define VER_NEED_NUM     2              // Given version number  

// Auxiliary needed version information.
typedef struct
{
    Elf32_Word  vna_hash;               // Hash value of dependency name
    Elf32_Half  vna_flags;              // Dependency specific information
    Elf32_Half  vna_other;              // Unused
    Elf32_Word  vna_name;               // Dependency name string offset
    Elf32_Word  vna_next;               // Offset in bytes to next vernaux  entry
} Elf32_Vernaux;

// Legal values for vna_flags.
#define VER_FLG_WEAK    0x2             // Weak version identifier  

// Auxiliary vector.
// This vector is normally only used by the program interpreter.  The
//  usual definition in an ABI supplement uses the name auxv_t.  The
//  vector is not usually defined in a standard &lt;elf.h&gt; file, but it
//  can't hurt.  We rename it to avoid conflicts.  The sizes of these
//  types are an arrangement between the exec server and the program
//  interpreter, so we don't fully specify them here.

typedef struct
{
    int a_type;                         // Entry type
    union
    {
        long int a_val;           // Integer value
        void *a_ptr;                      // Pointer value
        void (*a_fcn)(void);      // Function pointer value
    } a_un;
} Elf32_auxv_t;

// Legal values for a_type (entry type).
#define AT_NULL         0               // End of vector  
#define AT_IGNORE       1               // Entry should be ignored  
#define AT_EXECFD       2               // File descriptor of program  
#define AT_PHDR         3               // Program headers for program  
#define AT_PHENT        4               // Size of program header entry  
#define AT_PHNUM        5               // Number of program headers  
#define AT_PAGESZ       6               // System page size  
#define AT_BASE         7               // Base address of interpreter  
#define AT_FLAGS        8               // Flags  
#define AT_ENTRY        9               // Entry point of program  
#define AT_NOTELF       10              // Program is not ELF  
#define AT_UID          11              // Real uid  
#define AT_EUID         12              // Effective uid  
#define AT_GID          13              // Real gid  
#define AT_EGID         14              // Effective gid  

// Some more special a_type values describing the hardware.
#define AT_PLATFORM     15              // String identifying platform.   
#define AT_HWCAP        16              // Machine dependent hints about processor capabilities.   

// This entry gives some information about the FPU initialization performed by the kernel.
#define AT_FPUCW        17              // Used FPU control word.   

// Note section contents.  Each entry in the note section begins with
//   a header of a fixed form.
typedef struct
{
    Elf32_Word n_namesz;                        // Length of the note's name.
    Elf32_Word n_descsz;                        // Length of the note's descriptor.
    Elf32_Word n_type;                  // Type of the note.
} Elf32_Nhdr;

// Known names of notes.
// Solaris entries in the note section have this name.
#define ELF_NOTE_SOLARIS        "SUNW Solaris"

// Note entries for GNU systems have this name.
#define ELF_NOTE_GNU            "GNU"

// Defined types of notes for Solaris.
// Value of descriptor (one word) is desired pagesize for the binary.
#define ELF_NOTE_PAGESIZE_HINT  1

// Defined note types for GNU systems.
// ABI information.  The descriptor consists of words:
//  word 0: OS descriptor
//  word 1: major version of the ABI
//  word 2: minor version of the ABI
//  word 3: subminor version of the ABI

#define ELF_NOTE_ABI                    1

// Known OSes.  These value can appear in word 0 of an ELF_NOTE_ABI note section entry.
#define ELF_NOTE_OS_LINUX               0
#define ELF_NOTE_OS_GNU                 1
#define ELF_NOTE_OS_SOLARIS2            2

// Values for `kind' field in Elf_Options.
#define ODK_NULL                0       // Undefined.   
#define ODK_REGINFO             1       // Register usage information.   
#define ODK_EXCEPTIONS          2       // Exception processing options.   
#define ODK_PAD                 3       // Section padding options.   
#define ODK_HWPATCH             4       // Hardware workarounds performed  
#define ODK_FILL                5       // record the fill value used by the linker.  
#define ODK_TAGS                6       // reserve space for desktop tools to write.  
#define ODK_HWAND               7       // HW workarounds.  'AND' bits when merging.  
#define ODK_HWOR                8       // HW workarounds.  'OR' bits when merging.   

// Values for `info' in Elf_Options for ODK_EXCEPTIONS entries.
#define OEX_FPU_MIN             0x1f    // FPE's which MUST be enabled.   
#define OEX_FPU_MAX             0x1f00  // FPE's which MAY be enabled.   
#define OEX_PAGE0               0x10000 // page zero must be mapped.   
#define OEX_SMM                 0x20000 // Force sequential memory mode?   
#define OEX_FPDBUG              0x40000 // Force floating point debug mode?   
#define OEX_PRECISEFP           OEX_FPDBUG
#define OEX_DISMISS             0x80000 // Dismiss invalid address faults?   

#define OEX_FPU_INVAL           0x10
#define OEX_FPU_DIV0            0x08
#define OEX_FPU_OFLO            0x04
#define OEX_FPU_UFLO            0x02
#define OEX_FPU_INEX            0x01

// Masks for `info' in Elf_Options for an ODK_HWPATCH entry.
#define OHW_R4KEOP              0x1     // R4000 end-of-page patch.   
#define OHW_R8KPFETCH           0x2     // may need R8000 prefetch patch.   
#define OHW_R5KEOP              0x4     // R5000 end-of-page patch.   
#define OHW_R5KCVTL             0x8     // R5000 cvt.[ds].l bug.  clean=1.   

#define OPAD_PREFIX             0x1
#define OPAD_POSTFIX            0x2
#define OPAD_SYMBOL             0x4

// Entry found in `.options' section.
typedef struct
{
    Elf32_Word hwp_flags1;      // Extra flags.
    Elf32_Word hwp_flags2;      // Extra flags.
} Elf_Options_Hw;

// Masks for `info' in ElfOptions for ODK_HWAND and ODK_HWOR entries.
#define OHWA0_R4KEOP_CHECKED    0x00000001
#define OHWA1_R4KEOP_CLEAN      0x00000002

// Entries found in sections of type SHT_MIPS_CONFLICT.
typedef Elf32_Addr Elf32_Conflict;

// ARM specific declarations
// Processor specific flags for the ELF header e_flags field.
#define EF_ARM_RELEXEC     0x01
#define EF_ARM_HASENTRY    0x02
#define EF_ARM_INTERWORK   0x04
#define EF_ARM_APCS_26     0x08
#define EF_ARM_APCS_FLOAT  0x10
#define EF_ARM_PIC         0x20
#define EF_ALIGN8          0x40         // 8-bit structure alignment is in use  
#define EF_NEW_ABI         0x80
#define EF_OLD_ABI         0x100

// Additional symbol types for Thumb
#define STT_ARM_TFUNC      0xd

// ARM-specific values for sh_flags
#define SHF_ARM_ENTRYSECT  0x10000000   // Section contains an entry point  
#define SHF_ARM_COMDEF     0x80000000   // Section may be multiply defined
//in the input to a link step
// ARM-specific program header flags
#define PF_ARM_SB          0x10000000   // Segment contains the location

#define         EEXF_TYPE_NORMAL         0

#define         EEXF_MAGIC0             'e'
#define         EEXF_MAGIC1             'P'
#define         EEXF_MAGIC2             'D'
#define         EEXF_MAGIC3             'K'
#define         EEXF_MAGIC4             '.'
#define         EEXF_MAGIC5             'e'
#define         EEXF_MAGIC6             'x'
#define         EEXF_MAGIC7             'f'

#endif  // _EELF_H_  
