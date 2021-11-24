//
// Created by joseph on 22/11/2021.
//

#include "public/general/config.hpp"
#include "../../../logger/public/logger.hpp"

namespace flow
{

	void InflateFromJson(Config* config, const json& j)
	{
		logger::info("Starting config inflation");
		config->config_name = j["config_name"];
		config->time = j["time"];
		config->mod_key = j["mod_key"];
		config->fonts = j["fonts"].get<std::vector<std::string>>();
		config->key_bindings = std::vector<KeyBinding>();
		for (const auto& item: ((json::array_t)j["key_bindings"]))
		{
			const auto kb = KeyBinding(item["mod_key"], item["key"], item["function"], item["arg"], item["client"] == 0);
			config->key_bindings.push_back(kb);
		}

		logger::success("Config", config->config_name, "successfully read");
		logger::info("Value:\n", *config);
	}

	Config::Config(const std::string& file_name)
	{
		std::ifstream file(file_name);
		json j;

		try
		{
			file >> j;
		}
		catch (const std::exception& e)
		{
			logger::error(e.what());
			logger::error("The file", file_name, "is not a valid config!");
			return;
		}

		InflateFromJson(this, j);
	}

	std::ostream& operator<<(std::ostream& os, const Config& config)
	{
		os << "name: " << config.config_name << std::endl
		   << "time: " << config.time << std::endl
		   << "mod key: " << config.mod_key << std::endl << std::endl;

		for (const auto& item: config.key_bindings)
		{
			std::cout << "Key bindings:" << std::endl << item << std::endl << std::endl;
		}
		return os;
	}

}
