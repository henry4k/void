#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <void/Resource.h>
#include <void/Squirrel.h>


/**
 * A fixed timestep.
 */
typedef unsigned int Tick;

/**
 * Time between simulation ticks.
 */
static const float ServerTimestep = 1.0f; // 0.015f // Simulate the world every 15 milliseconds ( = 1 Tick = 66.66 times per second. )

/**
 * Ticks between update packets.
 */
static const Tick UpdateRate = 4; // 20 // Send updates every 20 ticks. (3.33 times per second.)

static const Tick GenesisTick = 1; // When stuff is created


class Engine
{
	public:
		Engine();
		virtual ~Engine();
		
		/**
		 * Does all the things that should be done regulary.
		 * Like drawing the scene and simulating the world.
		 * @see onUpdate
		 */
		bool update();
		
		/**
		 * A package is folder that contains a compilation of ressources and scripts.
		 * This function loads a package and tries to run main.nut in it.
		 */
		bool loadPackage( const char* name );
		
	protected:
		virtual bool onUpdate( double timeDelta ) = 0;
		
		ResourceManager m_ResourceManager;
		Squirrel m_Squirrel;

	private:
		double m_CurrentTime;
		double m_LastTime;
};

#endif
