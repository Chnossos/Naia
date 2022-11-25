#ifndef CONNECTIONTHREAD_HPP_INCLUDED
#define CONNECTIONTHREAD_HPP_INCLUDED

template <typename T> class Connector
{
	public:
		Connector (const io_ptr & ios, int port) :
			_ios (ios),
			_acceptor (*ios),
			_socket (new tcp::socket (*ios))
		{
			system::error_code ec;
			tcp::endpoint endpoint (tcp::v4(), port);
			_acceptor.open (endpoint.protocol());
			_acceptor.set_option (socket_base::reuse_address(false));
			_acceptor.bind (endpoint, ec);
			_acceptor.listen (socket_base::max_connections, ec);
			if (ec)
			{
				kernel.log.print_error ("Port %u is already in use !\n", port);
				_acceptor.close();
			}
		}

		~Connector () { delete _socket; }

		void handle_accept_connection (const boost::system::error_code & e)
		{
			if (e)
			{
				if (e.value() != 1236 && e.value() != 995)
					kernel.log.print_error ("[Con](%u) %s\n", e.value(), e.message().c_str());
				_acceptor.close();
				return;
			}

			if (!_ios->stopped())
			{
				T * t = new T (_socket);
				t->run();

				_socket = new tcp::socket (*_ios);
				run();
			}
		}

		void run ()
		{
			if (!_ios->stopped())
				_acceptor.async_accept (*_socket, boost::bind (&Connector::handle_accept_connection, this, asio::placeholders::error));
		}

		bool is_open () const { return _acceptor.is_open(); }

	private:
		io_ptr _ios;
		tcp::acceptor _acceptor;
		tcp::socket * _socket;
};

#endif // CONNECTIONTHREAD_HPP_INCLUDED
