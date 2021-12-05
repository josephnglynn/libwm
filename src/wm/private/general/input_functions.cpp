//
// Created by joseph on 27/11/2021.
//


#include "../../public/general/input_functions.hpp"
#include "../../../logger/public/logger.hpp"

namespace flow::input_functions
{

	std::function<void(const std::string&)> InputFunctionToFunctionPointer(InputFunction i_f)
	{
		switch (i_f)
		{
		case Spawn:
			return &spawn;
			break;
		}
	}

	std::vector<std::string> split(std::string s)
	{

		std::vector<std::string> strings;
		while (!s.empty()) {

			size_t pos = s.find(' ');
			if (pos == 0) {
				s.erase(s.begin(), s.begin() + 1);
				continue;
			}

			if (pos == std::string::npos) {
				strings.push_back(s);
				break;
			}

			strings.push_back(s.substr(0, pos));
			s.erase(s.begin(), s.begin() + pos);

		}

		return strings;
	}

	void spawn(const std::string& arg)
	{
		auto fwm = X11::FlowWindowManagerX11::Get();
		if (fork() == 0)
		{
			fwm->Detach();
			setsid();
			std::vector<std::string> split_string = split(arg);
			char** args = new char* [split_string.size() + 1];
			for (size_t i = 0; i < split_string.size(); ++i) {
				args[i] = strdup(split_string[i].c_str());
			}
			args[split_string.size()] = nullptr;
			execvp(args[0], args);
			_exit(0);
		}

	}
}

