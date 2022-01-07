//
// Created by joseph on 22/11/2021.
//

#include "../../public/general/config.hpp"
#include "../../../external/logger/src/public/logger.hpp"

namespace flow
{

	Config* InflateFromJson(const json& j)
	{
		auto config = new Config();
		logger::info("Starting config inflation");
		j.at("config_name").get_to(config->config_name);
		j.at("time").get_to(config->time);
		j.at("mod_key").get_to(config->mod_key);
		j.at("fonts").get_to(config->fonts);
		j.at("snap").get_to(config->snap);
		j.at("border_size").get_to(config->border_size_in_pixels);
		j.at("shell_location").get_to(config->shell_location);
		config->key_bindings = std::vector<KeyBinding>();
		config->client_key_bindings = std::vector<ClientKeyBinding>();

		for (const auto& item: ((json::array_t)j["key_bindings"]))
		{
			const auto kb = KeyBinding(item["mod"], item["key"], item["function"], item["arg"]);
			config->key_bindings.push_back(kb);
		}

		for (const auto& item: ((json::array_t)j["client_key_bindings"]))
		{
			const auto ckb =
				ClientKeyBinding(item["click"], item["event_mask"], item["button"], item["function"], item["arg"]);
			config->client_key_bindings.push_back(ckb);
		}

		logger::success("Config", config->config_name, "successfully read");
		logger::info("", *config);
		return config;
	}

	Config* ParseConfigFromFile(std::ifstream& file, const std::string& file_name)
	{
		Config* config;
		json j;

		try
		{
			file >> j;
			config = InflateFromJson(j);
		}
		catch (const std::exception& e)
		{
			logger::error(e.what());
			logger::error("The file", file_name, "is not a valid config!");
		}

		return config;
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

		for (const auto& item: config.client_key_bindings)
		{
			std::cout << "Client bindings:" << std::endl << item << std::endl << std::endl;
		}

		return os;
	}

	const std::array<const char*, 2> location = { "~/config/flow-wm/config.json", "/etc/flow-wm/config.json" };
	Config* Config::GetDefault()
	{
		Config* config;
		for (const auto& item: location)
		{
			std::ifstream file(item);
			if (file.good())
			{
				config = ParseConfigFromFile(file, item);
				logger::error("Oh dear... there was an error in", item);
				if (config) return config;
			}
			else
			{
				logger::info("Hmm...", item, "does not exist");
			}
		}
		throw std::runtime_error("Config files both had errors");
	}

	Config* Config::FromFilePath(const std::string& file_path)
	{
		std::ifstream file(file_path);
		return ParseConfigFromFile(file, file_path);
	}

}
