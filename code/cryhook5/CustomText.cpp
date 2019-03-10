
/*
*  This source file is part of the Far Cry 5 ScriptHook by Force67
*  More information regarding licensing can be found in LICENSE.md
*/

#include <Hooking.h>
#include <Nomad/nomad_base_function.h>
#include <Utility/PathUtils.h>

#include <Minhook.h>

static std::unordered_map<uint64_t, std::wstring> g_overrides;

static FILE *dump_file = nullptr;
static bool ready = false;

static void *(*FetchTranslation_Orig)(__int64, __int64, uint32_t, __int64, char);
static const wchar_t *(*GetLocalizedText_Orig)(uint64_t);

static const wchar_t *GetLocalizedText(uint64_t hash)
{
	if (hash == 0x2357aa83760)
	{
		return L"CryHookV is installed";
	}

	if (ready)
	{
		auto it = g_overrides.find(hash);

		if (it != g_overrides.end())
		{
			return it->second.c_str();
		}	
	}

	auto text = GetLocalizedText_Orig(hash);

	std::wprintf(L"%llx, %s\n", hash, text);

	if (dump_file && text)
	{
		fwprintf(dump_file, L"%llx, %s\n", hash, text);
	}

	return text;
}

template <typename T>
struct NdStringBuffer
{
	//DWORD ukn;
	//DWORD max_len;
	//char pad[4];
	char pad[8];
	T *ptr;
};

template <typename T>
struct NdString
{
	bool initialized;
	char pad[7];
	NdStringBuffer<T> *buffer;

	inline T *c_str()
	{
		return buffer->ptr;
	}
};

using WideNdString = NdString<wchar_t>;
using AnsiNdString = NdString<char>;

static bool (*MakeNdString)(WideNdString*);
static WideNdString* (*SetNdString)(WideNdString*, const wchar_t*, size_t);

static WideNdString*(*Internal_FetchTranslation_Orig)(void*, __int64, unsigned int);

static WideNdString* Internal_FetchTranslation_Stub(void* a1, __int64 a2, unsigned int hash)
{
	auto ndstring = Internal_FetchTranslation_Orig(a1, a2, hash);

	auto wchar = (wchar_t *)(*(__int64 *)(ndstring + 8) + 12i64);

	printf("Game access string %x, %S, %S\n", hash, wchar, ndstring->c_str());

	if (hash == 0xaeab2)
	{
		wcscpy(wchar, L"force was here!");
	}

	return ndstring;
}

static WideNdString*(*VeryInternal_FetchTranslation_Orig)(void*, char*, uint32_t, uint32_t, int32_t);

static WideNdString* VeryInternal_FetchTranslation_Stub(void* mgr, char* out_nd_str, uint32_t ukn, uint32_t hash, int32_t ukn2)
{
	if (hash == 0xaeab2)
	{
		// if we initialized our string correctly
		if (MakeNdString((WideNdString*)out_nd_str))
		{
			printf("Out %p\n", out_nd_str);

			*(bool*)out_nd_str = true;

			// copy our text on the string
			const wchar_t *tettxt = L"test";

			SetNdString((WideNdString*)out_nd_str, tettxt, wcslen(tettxt));

			auto wchar = (wchar_t *)(*(__int64 *)(out_nd_str + 8) + 12i64);

			printf("Game copied string %x, %S\n", hash, wchar);
		}

		return (WideNdString*)out_nd_str;
	}

	return VeryInternal_FetchTranslation_Orig(mgr, out_nd_str, ukn, hash, ukn2);
}

/*
 *   do
  {
    v25 = *v24;
    ++v24;
  }
  while ( v25 );
  FUNC_SomeTHingCopyNDSTRING((__int64)&byte_184EA8EB0, L"glider", (unsigned int)(v24 - L"glider") - 1);
  v183 = FUNC_CreateGameString((__int64)&v183);
  byte_184EA8EC0 = FUNC_CreateGameString((__int64)&byte_184EA8EC0);
  v26 = L"moto";
  qword_184EA8EC8 = 0i64;
  do
  {
    v27 = *v26;
 */

static nomad::base_function init([]()
{
		return;

	auto dump = Utility::MakeAbsolutePathW(L"cryhook5\\strdump.txt");

	//if (wcsstr(GetCommandLineW(), L"-strdump"))
	if (GetFileAttributesW(dump.c_str()) == INVALID_FILE_ATTRIBUTES)
	{
		_wfopen_s(&dump_file, dump.c_str(), L"w");

		if (dump_file)
		{
			std::atexit([]()
			{
				if (dump_file)
					fclose(dump_file);
			});
		}
	}

	auto db = Utility::MakeAbsolutePathW(L"cryhook5\\stringdb.txt");

	FILE *fh = nullptr;
	_wfopen_s(&fh, db.c_str(), L"r");

	if (fh)
	{
		ready = true;
		
		uint64_t hash = 0;
		wchar_t buf[2048] = { 0 };

		// parse the db
		while (fwscanf(fh, L"%llx, %1024[^\n]\n", &hash, &buf) != EOF)
		{
			g_overrides.insert(std::make_pair(hash, std::wstring(buf)));

			std::printf("inserted %llx %S\n", hash, buf);
		}

		fclose(fh);
	}

#if 0
	// hook the localized string getter function
	char *loc = nio::pattern("53 48 83 EC 20 48 83 79 ? ? 48 89 CB 75 44").first();

	MH_CreateHook(loc, &GetLocalizedText, (LPVOID*)&GetLocalizedText_Orig);
	MH_EnableHook(loc);
#endif

	// somewhere deep in the translation engine
	//char *loc = nio::pattern("53 48 83 EC 30 45 89 C1 C7 44 24 ? ? ? ? ? 45 31 C0").first();

	//MH_CreateHook(loc, (LPVOID)&Internal_FetchTranslation_Stub, (LPVOID*)&Internal_FetchTranslation_Orig);
	//MH_EnableHook(loc);

	char *loc = nio::pattern("55 56 57 41 54 41 55 41 56 41 57 48 83 EC 40 48 8B 79 10").first(-10);
	MH_CreateHook(loc, (LPVOID)&VeryInternal_FetchTranslation_Stub, (LPVOID*)&VeryInternal_FetchTranslation_Orig);
	MH_EnableHook(loc);

	// lets hope this doesnt break
	loc += 0x9C;
	nio::set_call(&MakeNdString, loc);

	loc += 0xCD;
	nio::set_call(&SetNdString, loc);
});
