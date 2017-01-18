#include "stdafx.h"

#include <windows.h>

#include "CuiChatParser.h"
#include "CuiMediatorFactory.h"
#include "SwgCuiConsole.h"
#include "EmuCommandParser.h"

template<typename String, typename Delimiter, typename Vector>
void split(const String& s, Delimiter delim, Vector& v) {
	auto i = 0;
	auto pos = s.find(delim);
	while (pos != String::npos) {
		v.push_back(s.substr(i, pos - i));
		i = ++pos;
		pos = s.find(delim, pos);

		if (pos == String::npos)
			v.push_back(s.substr(i, s.length()));
	}
}


bool CuiChatParser::parse(const soe::unicode& incomingCommand, soe::unicode& resultUnicode, uint32_t chatRoomID, bool useChatRoom) {
	CuiMediator* console = CuiMediatorFactory::get("Console");

	bool consoleActive = console ? console->isActive() : false;

	if (consoleActive)
		resultUnicode += (L"\\#ffffff > \\#888888" + incomingCommand + L"\\#ffffff\n");

	auto foundSlash = incomingCommand.find(L"/");
	if (foundSlash != soe::unicode::npos) {
		// Strip the slash.
		auto command = incomingCommand.substr(foundSlash + 1);

		if (command == L"console") {
			if (console == nullptr) {
				typedef CuiMediatorFactory::Constructor<SwgCuiConsole> ctor_t;

				auto ctor = new ctor_t(("/Console"));
				CuiMediatorFactory::addConstructor("Console", ctor);

				console = CuiMediatorFactory::get("Console");

				if (console != nullptr) {
					resultUnicode += L"swgemu console installed";

					CuiMediatorFactory::activate("Console");
				} else {
					resultUnicode += "could not find console in cui mediator factory";
				}
			} else {
				CuiMediatorFactory::toggle("Console");
			}

			return true;
		} else if (command == L"exit") {
			if (consoleActive) {
				CuiMediatorFactory::toggle("Console");

				return true;
			}
		} else if ((command.size() == 3 && command == L"emu") || (command.find(L"emu ") == 0)) {
			if (!consoleActive) {
				soe::vector<soe::unicode> args;
				split(command, L' ', args);

				EmuCommandParser::parse(args, command, resultUnicode);

				return true;
			}
		}
	}

	return originalParse::run(incomingCommand, resultUnicode, chatRoomID, useChatRoom);
}
