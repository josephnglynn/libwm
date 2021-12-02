//
// Created by joseph on 27/11/2021.
//

#include "general/input_functions.hpp"

namespace flow::input_functions
{


	std::function<void(const std::string&)> InputFunctionToFunctionPointer(InputFunction i_f) {
		switch (i_f)
		{
		case Spawn:
			return &spawn;
			break;
		}
	}


	char* const* split(std::string s)
	{
		unsigned long size = 0;
		std::vector<std::string> strings;
		long pos = 1;
		while (pos)
		{
			pos = static_cast<long>(s.find(' '));
			strings.emplace_back(s.begin(), s.begin() + pos);
			size += strings.back().size();
			s.erase(s.begin(), s.begin() + pos);
		}

		char** array = static_cast<char**>(malloc(size * sizeof(char*)));
		for (unsigned long i = 0; i < strings.size(); ++i)
		{
			array[i] = static_cast<char*>(malloc((strings[i].size() + 1) * sizeof(char)));
			array[i] = (char*)strings[i].c_str();
		}

		return array;
	}

	void spawn(const std::string& arg)
	{
		auto fwm = X11::FlowWindowManagerX11::Get();
		if (fork() == 0)
		{
			if (fwm->GetDisplay())
			{
				close(ConnectionNumber(fwm->GetDisplay()));
			}
			setsid();
			auto pos = arg.find(' ');
			execvp(arg.substr(0, pos).c_str(), split(arg.substr(pos)));
		}

	}
}

