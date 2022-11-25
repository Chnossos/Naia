#ifndef PACKET_HPP_INCLUDED
#define PACKET_HPP_INCLUDED

class Packet
{
	public:
		explicit Packet (const string & packet = "") { _stream << packet; }
		explicit Packet (SMessage packet)
		{
			_stream << Naia::hex_to_string (Naia::to_hex_string (packet));
		}
		explicit Packet (SSMessage packet)
		{
			_stream << Naia::hex_to_string (Naia::to_hex_string (packet));
		}
		Packet (const Packet & packet)
		{
			_stream.str (packet.to_string());
		}

		template <typename T> Packet & operator<< (const T & val)
		{
			_stream << val;
			return *this;
		}

		Packet & operator << (const SMessage & packet)
		{
			_stream << Naia::hex_to_string (Naia::to_hex_string (packet));
			return *this;
		}

		string to_string () const { return _stream.str(); }

	private:
		std::ostringstream _stream;
};

#endif // PACKET_HPP_INCLUDED
