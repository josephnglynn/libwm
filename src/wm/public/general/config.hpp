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
		Config(const std::string& file_name);

		std::string config_name;
		int64_t time;
		Key mod_key;
		std::vector<KeyBinding> key_bindings;


		friend std::ostream& operator<<(std::ostream& os, const Config& config);

	};

}

#endif //FLOW_WM_SRC_WM_PRIVATE_GENERAL_CONFIG_HPP
