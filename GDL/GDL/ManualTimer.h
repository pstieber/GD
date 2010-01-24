#ifndef MANUALTIMER_H
#define MANUALTIMER_H
#include <string>

using namespace std;
/**
 *  A manual timer.
 *  A manual timer is a timer which is updated manually
 *  by calling UpdateTime.
 */
class GD_API ManualTimer
{
    public:

        ManualTimer(string name_);
        virtual ~ManualTimer();
        /**
         * Get the name of the timer
         * @return Timer's name
         */
        inline string GetName() { return name; };

        /**
         * Update the time of the timer
         * @param Time to add in seconds
         */
        inline void UpdateTime(float time_) { if (!isPaused) time += time_; };

        /**
         * Reset time to zero.
         */
        inline void Reset() { time = 0; };

        /**
         * Get the current time elapsed.
         * @return Time elapsed
         */
        inline float GetTime() const { return time; };

        /**
         * Get the paused state of the timer.
         * @return true if paused
         */
        inline bool IsPaused() const { return isPaused; };

        /**
         * Set the paused state of the timer.
         * @param The new state ( true = paused )
         */
        inline void SetPaused(bool newState = true) { isPaused = newState; };

    protected:
    private:

        string name;
        float time;
        bool isPaused;
};

#endif // MANUALTIMER_H