#ifndef __ENGINE_H__
#define __ENGINE_H__


typedef unsigned int Tick;

// static const float ServerTimestep = 0.015f; // Simulate the world every 15 milliseconds. ( = 1 Tick = 66.66 times per second. )
// static const Tick UpdateRate = 20; // Send updates every 20 ticks. (3.33 times per second.)
static const float ServerTimestep = 1.0f;
static const Tick UpdateRate = 4;

static const Tick GenesisTick = 1; // When stuff is created


class Engine
{
	public:
		Engine();
		virtual ~Engine();

		void simulate();
		
	protected:
		bool loadPackage( const char* name );
		
	private:
		double m_CurrentTime;
		double m_LastTime;
};

#endif