//
// Created by joseph on 22/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PRIVATE_GENERAL_CONFIG_HPP
#define FLOW_WM_SRC_WM_PRIVATE_GENERAL_CONFIG_HPP
#include "keybinding.hpp"
#include <nlohmann/json.hpp>
#include <chrono>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace flow
{

	struct Config
	{
		Config() = default;
		static Config* GetDefault();
		static Config* FromFilePath(const std::string& file_path);

		std::string config_name;
		int64_t time;
		Key mod_key;
		unsigned int snap;
		unsigned int border_size_in_pixels;
		std::string shell_location;
		std::vector<KeyBinding> key_bindings;
		std::vector<ClientKeyBinding> client_key_bindings;
		std::vector<std::string> fonts;

		friend std::ostream& operator<<(std::ostream& os, const Config& config);

	};

}

#endif //FLOW_WM_SRC_WM_PRIVATE_GENERAL_CONFIG_HPP
