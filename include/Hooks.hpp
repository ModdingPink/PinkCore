// Implementation by https://github.com/StackDoubleFlow
// yoinked from 
#pragma once
#include "beatsaber-hook/shared/utils/logging.hpp"
#include <vector>

class Hooks 
{
	private:
		static std::vector<void(*)(Logger& logger)> installFuncs;
	public:
		static void AddInstallFunc(void(*installFunc)(Logger& logger)) 
		{
			installFuncs.push_back(installFunc);
		}

		static void InstallHooks(Logger& logger)
		{
			for (auto installFunc : installFuncs)
			{
				installFunc(logger);
			}
		}
};

#define PCInstallHooks(func) \
struct __PCRegister##func { \
	__PCRegister##func() { \
		Hooks::AddInstallFunc(func); \
	} \
}; \
static __PCRegister##func __PCRegisterInstance##func;

#define SIMPLE_INSTALL_HOOK(identifier) INSTALL_HOOK(logger, identifier);
#define SIMPLE_INSTALL_HOOK_ORIG(identifier) INSTALL_HOOK_ORIG(logger, identifier);