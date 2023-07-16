#include <chrono>
#include <cfloat>

/**
 * A time tracking structure.
 */
class Timer
{
	using clock = std::chrono::steady_clock;
  clock::time_point tpstart{};
  double tmlim{DBL_MAX};
  
public: 
	Timer(const double tl_sec = DBL_MAX)
	{
		Reset(tl_sec);
	}

	void Reset(const double tl_sec = DBL_MAX)
	{
    tmlim = tl_sec;
    tpstart = clock::now();
	}

	double Elapsed() const
	{
		using namespace std::chrono;
		return duration_cast<duration<double>>(clock::now() - tpstart).count();
	}
  
	bool TimedOut() const { return Elapsed() >= tmlim; }
};