/*
 * This file is part of the CitizenFX project - http://citizen.re/
 *
 * See LICENSE and MENTIONS in the root of the source tree for information
 * regarding licensing.
 */

#include "Hooking.h"

namespace hook
{
    static uintptr_t g_currentStub = 0;

#ifndef _M_AMD64
	void inject_hook::inject()
	{
		inject_hook_frontend fe(this);
		m_assembly = std::make_shared<FunctionAssembly>(fe);

		writeVP<uint8_t>(m_address, 0xE9);
        writeVP<int>(m_address + 1, (uintptr_t)m_assembly->GetCode() - (uintptr_t)get_adjusted(m_address) - 5);
	}

	void inject_hook::injectCall()
	{
		inject_hook_frontend fe(this);
		m_assembly = std::make_shared<FunctionAssembly>(fe);

        writeVP<uint8_t>(m_address, 0xE8);
        writeVP<int>(m_address + 1, (uintptr_t)m_assembly->GetCode() - (uintptr_t)get_adjusted(m_address) - 5);
	}
#else
	void* AllocateFunctionStub(void* ptr, int type)
	{
        if (!g_currentStub)
        {
            auto base = GetModuleHandle(L"FC_m64.dll");
            auto hdr = (PIMAGE_DOS_HEADER)base;
            auto nt_header = PIMAGE_NT_HEADERS(&base[hdr->e_lfanew]);
            g_currentStub = (uintptr_t)(base + 6000000 + 250);
        }

        char* code = (char*)g_currentStub;

        DWORD oldProtect;
        VirtualAlloc(code, 20, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        VirtualProtect(code, 15, PAGE_EXECUTE_READWRITE, &oldProtect);

        *(uint8_t*)code = 0x48;
        *(uint8_t*)(code + 1) = 0xb8 | type;
        *(uint64_t*)(code + 2) = (uint64_t)ptr;
        *(uint16_t*)(code + 10) = 0xE0FF | (type << 8);
        *(uint64_t*)(code + 12) = 0xCCCCCCCCCCCCCCCC;
        g_currentStub += 20;

        return code;

	}
#endif
}