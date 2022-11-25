#ifndef SINGLETON_H_INCLUDED
#define SINGLETON_H_INCLUDED

#define initialize_singleton( type ) \
  template <> type * Singleton < type > :: _singleton = NULL

template <typename T> class Singleton
{
	public:
		static T * instance_ptr ()
		{
			if (_singleton == NULL)
				_singleton = new T;
			return (static_cast <T *> (_singleton));
		}

		static T & instance ()
		{
			if (_singleton == NULL)
				_singleton = new T;
			return *(static_cast <T *> (_singleton));
		}

		static void release ()
		{
            if (_singleton != NULL)
            {
                delete _singleton;
                _singleton = NULL;
            }
		}

		static bool is_instanciated ()
		{
			return _singleton == NULL ? false : true;
		}

	protected:
		Singleton () {}
		virtual ~Singleton () {}

	private:
		static T * _singleton;
};

#endif
