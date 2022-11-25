#ifndef PROGRESSBAR_H_INCLUDED
#define PROGRESSBAR_H_INCLUDED

#ifdef _WIN32
#define full "\x3D"
#else
#define full "="
#endif

class ProgressBar
{
	public:
		explicit ProgressBar (int stp_count);
		~ProgressBar ();
		void step ();

	private:
		int lenght;
		int step_count;
		int current_pos;
		int current_step;
};

class ProgressPercent
{
	public:
		explicit ProgressPercent (int stp_count);
		~ProgressPercent ();
		void step ();

	private:
		int step_count;
        int last_percent;
		int current_pos;
		int current_step;
};

#endif
