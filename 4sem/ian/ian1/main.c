#include "printing.h"
#include <elf.h>
#include <fcntl.h>
#include <gelf.h>
#include <libelf.h>
#include <stdio.h>
#include <unistd.h>

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

/// finds the section header of the symbol table (type SHT_SYMTAB),
/// returns NULL on error
Elf_Scn *find_symtab(Elf *elf_file) {
    Elf_Scn *section = NULL;

    while ((section = elf_nextscn(elf_file, section)) != NULL) {
        Elf64_Shdr *section_header = elf64_getshdr(section);

        if (section_header == NULL) {
            eprintf("Cannot find section header: %s\n", elf_errmsg(-1));
            return NULL;
        }

        if (section_header->sh_type == SHT_SYMTAB) {
            return section;
        }
    }

    // no symbol table found
    eprintf("No symbol table found");
    return NULL;
}

/// parses argv, opens provided file, returns file descriptor, or -1 on error
int open_file(int argc, char **argv) {
    if (argc != 2) {
        eprintf("Usage: %s <file>\n", argv[0]);
        return -1;
    }

    int file_fd = open(argv[1], O_RDONLY);

    // open file with name at argv[1]
    if (file_fd < 0) {
        eprintf("Cannot open file '%s'\n", argv[1]);
        return -1;
    }

    return file_fd;
}

/// initializes libelf, parses input file as ELF, checks format,
/// returns Elf* handle, or NULL on error
Elf *parse_elf(int elf_file_fd) {
    if (elf_version(EV_CURRENT) == EV_NONE) {
        eprintf("libelf failed to initialize: %s\n", elf_errmsg(-1));
        return NULL;
    }

    Elf *elf_file = elf_begin(elf_file_fd, ELF_C_READ, NULL);

    // parse file using libelf
    if (elf_file == NULL) {
        eprintf("Not a valid ELF file: %s\n", elf_errmsg(-1));
        return NULL;
    }

    // check that file is actually an ELF file (libelf supports multiple
    // formats)
    if (elf_kind(elf_file) != ELF_K_ELF) {
        eprintf("Not an ELF file\n");
        elf_end(elf_file);
        return NULL;
    }

    return elf_file;
}

int main(int argc, char **argv) {
    int return_code = 0;

    // open file
    int elf_file_fd = open_file(argc, argv);

    if (elf_file_fd < 0) {
        return 1;
    }

    // parse file as ELF
    Elf *elf_file = parse_elf(elf_file_fd);

    if (elf_file == NULL) {
        return_code = 1;
        goto ERR_CLOSE_FD;
    }

    // find symbol table
    Elf_Scn *symtab_section = find_symtab(elf_file);

    if (symtab_section == NULL) {
        return_code = 1;
        goto ERR_ELF_END;
    }

    // find and print names and values of all integer-sized symbols
    print_symbols(elf_file, symtab_section);

ERR_ELF_END:
    elf_end(elf_file);

ERR_CLOSE_FD:
    close(elf_file_fd);
    return return_code;
}
