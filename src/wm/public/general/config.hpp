//
// Created by joseph on 22/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PRIVATE_GENERAL_CONFIG_HPP
#define FLOW_WM_SRC_WM_PRIVATE_GENERAL_CONFIG_HPP
#include <nlohmann/json.hpp>
#include <chrono>
#include "keybinding.hpp"

namespace flow
{


	class Config
	{
	public:
		static Config fromFile() {

		}

	private:
		std::string configName;
	};
}

#endif //FLOW_WM_SRC_WM_PRIVATE_GENERAL_CONFIG_HPP
