#include <pch.hpp>

#include <DbgHelp.h>

#pragma comment(lib, "Dbghelp.lib")

#include <hooks/anticheat/symbols.hpp>

std::shared_ptr<syms::parser_t> syms::g_parser;

std::string syms::parser_t::get_function_sym_by_address(void* address, uint64_t* offset_into_function_output) {
    uint8_t buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
    RtlZeroMemory(buffer, sizeof(buffer));

    auto symbol_information = reinterpret_cast<PSYMBOL_INFO>(buffer);
    symbol_information->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol_information->MaxNameLen = MAX_SYM_NAME;

    DWORD64 offset_into_function = 0;
    auto result = SymFromAddr(reinterpret_cast<HANDLE>(-1), reinterpret_cast<DWORD64>(address), &offset_into_function, symbol_information);
    if (!result) 
        return "Couldn't retrieve function name.";
    
    if (offset_into_function_output) 
        *offset_into_function_output = offset_into_function;   

    return std::string(symbol_information->Name);
}

uintptr_t syms::parser_t::get_function_start(void* address) {
    uint8_t buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
    RtlZeroMemory(buffer, sizeof(buffer));

    auto symbol_information = reinterpret_cast<PSYMBOL_INFO>(buffer);
    symbol_information->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol_information->MaxNameLen = MAX_SYM_NAME;

    DWORD64 offset_into_function = 0;
    auto result = SymFromAddr(reinterpret_cast<HANDLE>(-1), reinterpret_cast<DWORD64>(address), &offset_into_function, symbol_information);
    if (!result) return 0;

    return symbol_information->Address;
}

syms::parser_t::parser_t() {
    SymSetOptions(SYMOPT_UNDNAME);
    SymInitialize(reinterpret_cast<HANDLE>(-1), nullptr, true);
}

void syms::initialize() { g_parser = std::make_shared<parser_t>(); }
