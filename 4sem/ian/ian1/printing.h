#ifndef PRINTING_H
#define PRINTING_H

#include <elf.h>
#include <gelf.h>
#include <libelf.h>

/// finds all symbols of global scope and object type, of size
/// sizeof(int), and prints their name and value
void print_symbols(Elf *elf_file, Elf_Scn *symtab_section);

#endif // PRINTING_H
