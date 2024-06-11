#include <elf.h>
#include <gelf.h>
#include <libelf.h>
#include <stdio.h>

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

/// prints the name and value of symbol, in case of error, message
/// is printed to stderr, and function ends
void print_symbol(Elf *elf_file, Elf64_Shdr *symtab_section_header,
                  GElf_Sym symbol) {
    char *symbol_name =
        elf_strptr(elf_file, symtab_section_header->sh_link, symbol.st_name);

    if (symbol_name == NULL) {
        return;
    }

    // symbol's actual value is in another section, for which we
    // know the offset (symbol.st_shndx)
    Elf_Scn *value_section = elf_getscn(elf_file, symbol.st_shndx);

    if (value_section == NULL) {
        eprintf("Error getting section information: %s\n", elf_errmsg(-1));
        return;
    }

    Elf64_Shdr *value_section_header = elf64_getshdr(value_section);
    Elf_Data *value_section_data = elf_getdata(value_section, NULL);

    if (value_section_header == NULL || value_section_data == NULL) {
        eprintf("Error getting section information: %s\n", elf_errmsg(-1));
        return;
    }

    int symbol_value = 0;

    // compiler might put a global variable initialized to zero into the
    // .bss section, PROGBITS means there is actually a value
    if (value_section_header->sh_type == SHT_PROGBITS) {
        // type of pointer must be char*, all offsets are *byte* offsets
        char *data = (value_section_data->d_buf);

        // in DYN and EXEC file types, st_value contains virtual address
        // of the symbol => we must subtract the offset of the section
        data -= value_section_header->sh_addr;

        symbol_value = *(int *)(data + symbol.st_value);
    }

    printf("%s\t\t%d\n", symbol_name, symbol_value);
}

/// finds all symbols of global scope and object type, of size
/// sizeof(int), and prints their name and value
void print_symbols(Elf *elf_file, Elf_Scn *symtab_section) {
    puts("Name\t\tValue");

    Elf_Data *symbol_table_data = elf_getdata(symtab_section, NULL);
    Elf64_Shdr *symtab_section_header = elf64_getshdr(symtab_section);

    if (symbol_table_data == NULL || symtab_section_header == NULL) {
        return;
    }

    int symbol_count =
        symtab_section_header->sh_size / symtab_section_header->sh_entsize;

    for (int symbol_idx = 0; symbol_idx < symbol_count; symbol_idx++) {
        GElf_Sym symbol;

        if (gelf_getsym(symbol_table_data, symbol_idx, &symbol) == NULL) {
            eprintf("Error getting symbol information: %s\n", elf_errmsg(-1));
            continue;
        }

        if (
            // symbol has the size of int
            symbol.st_size == sizeof(int) &&
            // symbol has a name
            symbol.st_name != 0 &&
            // symbol is global, and an object
            ELF64_ST_BIND(symbol.st_info) == STB_GLOBAL &&
            ELF64_ST_TYPE(symbol.st_info) == STT_OBJECT &&
            symbol.st_shndx != SHN_ABS && symbol.st_shndx != SHN_COMMON &&
            symbol.st_shndx != SHN_UNDEF) {

            print_symbol(elf_file, symtab_section_header, symbol);
        }
    }
}
