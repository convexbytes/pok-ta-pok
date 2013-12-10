#include "client.h" 
#include "signal.h"
#include <iostream>
#include <boost/regex.hpp>

//Se utiliza boost para gestionar los parametros de ejecución del programa
#include <boost/program_options.hpp>
namespace po = boost::program_options;

// Se ejecutará esta función cuando presionen ctrl-c en terminal
void signalController( int num );

//Función para validar la ip del servidor
bool checkServerIP(std::string server);

int main( int ac, char * av[] )
{
	bool fail = false;
	std::string server;
	unsigned int port;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("server", po::value<std::string>(&server)->default_value("localhost"), "set server's IP")
		("port", po::value<unsigned int>(&port)->default_value(6000), "set server's port")
		("team_name", po::value<std::string>(), "set team's name")
		("goalie", "indicates if player is goalkeeper")
	;

	po::variables_map vm;        
	po::store(po::parse_command_line(ac, av, desc), vm);
	po::notify(vm);    

	if (vm.count("help"))
	{
		std::cout << desc << "\n";
		return 0;
	}

	if(!vm.count("team_name") || checkServerIP(vm["server"].as<std::string>()))
	{
		std::cout << desc << "\n";
		return 1;
	}

	//Asociamos nuestra función para manejar señales.
	signal( SIGINT, signalController );

	Client::instance().main_loop( vm["server"].as<std::string>(), vm["port"].as<unsigned int>(),  vm["team_name"].as<std::string>(), vm.count("goalie"));

	return 0;
}


void signalController(int num)
{
	//Este código se ejecutará cuando se reciba alguna señal.

	std::cout << "Signal received, exiting now..." << std::endl;
	exit(0);
}

bool checkServerIP(std::string server)
{
	if(server == "localhost")
		return false;

	const char* pattern =
		"\\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"
		"\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"
		"\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"
		"\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b";
		
	return !regex_match(server,boost::regex (pattern));
}
