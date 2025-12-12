#ifndef CONFIGPARS_HPP
#define CONFIGPARS_HPP

#include "StringUtils.hpp"
#include "Location.hpp"

#include "Server.hpp"

class ConfigPars : public StringUtils
{
	private:
		bool						_is_open_http;
		bool						_is_open_server;
		size_t						_ind;
		std::vector<Location>		_locations;
		std::vector<std::string>	_token_nl;
		std::vector<bool>			_is_open_location;
		std::vector<int>			_ports;
		std::vector<std::string>	_key_value;
		std::string					_file_content;
		std::string					_index;
		std::string					_root;
		std::string					_server_name;
		std::string					_body_max_size;
		std::string 				_error_404;
		std::string 				_error_500;
		std::string					_error_massage;
		ConfigPars();
		ConfigPars(const ConfigPars &);
		ConfigPars & operator = (const ConfigPars &);

		void	read_config_file(std::string &);
		void	create_server_class();
		void	join_location_path(std::string &);
		void	callFunctionByName(const std::string &);

		void	index();
		void	http_block();
		void	server_block();
		void	location_block();
		void	is_valid_line();
		void	listen();
		void	client_max_body_size();
		void	server_name();
		void	error_page_404();
		void	error_page_500();
		void	root();
		bool	check_config_blocks(const std::string &);
		
		void	close_blocks();

	public:
		std::vector<Server>	servers;
		~ConfigPars();
		ConfigPars(std::string &);
};

#endif
