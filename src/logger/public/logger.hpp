//
// Created by joseph on 18/11/2021.
//

#ifndef FLOW_WM_LOGGER_HPP
#define FLOW_WM_LOGGER_HPP

#define Red "\u001b[31m"
#define Green "\u001b[32m"
#define Yellow "\u001b[33m"
#define Blue "\u001b[34m"
#define RESET "\u001b[0m"
#define _starting_string "\u001b[37m==> "

#include <iostream>

namespace flow::logger
{

	template<typename ...T>
	void info(T... data)
	{

		std::cout << _starting_string << Blue;
		((std::cout << std::forward<T>(data) << " "), ...);
		std::cout << RESET << std::endl;
	}

	template<typename ...T>
	void warn(T... data)
	{
		std::cout << _starting_string << Yellow;
		((std::cout << std::forward<T>(data) << " "), ...);
		std::cout << RESET << std::endl;
	}

	template<typename ...T>
	void error(T... data)
	{
		std::cout << _starting_string << Red;
		((std::cout << std::forward<T>(data) << " "), ...);
		std::cout << RESET << std::endl;
	}

	template<typename ...T>
	void success(T... data)
	{
		std::cout << _starting_string << Green;
		((std::cout << std::forward<T>(data) << " "), ...);
		std::cout << RESET << std::endl;
	}

}

#endif //FLOW_WM_LOGGER_HPP
