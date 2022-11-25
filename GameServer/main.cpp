#include "Naia.hpp"

int main ()
{
	bool restart;
	do
	{
		restart = false;
		if (!Application::instance().run() || Application::instance().state() == APP_ERROR)
		{
			cerr << "\a" << endl;
			kernel.log.print_error ("Abnormal request for shutting down !");
			restart = false;
		}

		if (Application::instance().state() == APP_RESTART)
		{
			kernel.log.print_separator ("RESTART REQUESTED");
			restart = true;
		}
		else
			kernel.log.print_separator ("SHUTDOWN REQUESTED");

		Application::release();
	}
	while (restart);

	cout << "\nPress [ ENTER ] to close the program ...";
	getchar();

    return 0;
}
