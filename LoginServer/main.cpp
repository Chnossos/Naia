#include "Naia.hpp"

int main ()
{
	if (!Application::instance().run())
	{
		cerr << "\a";
		kernel.log.print_error ("Abnormal request for shutting down !");
	}

	kernel.log.print_separator ("SHUTDOWN REQUESTED");
	Application::release();

	cout << "\nPress [ ENTER ] to close the program ...";
	getchar();

    return 0;
}
