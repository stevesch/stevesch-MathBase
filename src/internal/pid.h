#ifndef INCLUDE_STEVESCH_MATHBASE_INTERNAL_PID_H_
#define INCLUDE_STEVESCH_MATHBASE_INTERNAL_PID_H_

#include "scalar.h"

namespace stevesch
{
#ifdef __cplusplus
	extern "C" {
#endif
  // Copyright © 2002, Stephen Schlueter, All Rights Reserved. https://github.com/stevesch
  
	// This file implements a PID motion controller
	//
	// Purpose:
	// The APID (proportional integrator and differentiator) produces
	// a smooth spring-like effect.  The effect can be adjusted with
	// 3 coefficients: f = a*s + b*(ds/dt) + c*(integral of s)
	// s:	offset from the equilibrium position (desired position)
	// a:	proportional term (like a spring, force is proportional to
	//		the offset from the equilibrium point)
	// b:	differential term (force produced by this term is proportional
	// 		to the velocity (i.e. fast-moving things get slowed down)
	// c:	integral term (forces long-lived non-zero offsets toward zero)
	//
	// How to use:
	//
	// 1. declare a variable of type APID (I'll refer to this variable as <pid>)
	// 2. call APIDInit(&<pid>, a, b, c) [reasonable values are a=0.08f, b=0.3f, c=0.000001f]
	// 3. call APIDAdvance(&<pid>) each frame
	//
	// - At any time, call APIDGetEq(&<pid>) to retrieve the current destination
	//
	// - At any time, call APIDSetEq(&<pid>, <equilibrium value>) to set a new destination.
	// Note: this resets internal APID states, so you should only call this when the
	// new destination is different than the current.  You can try something like this:
	//
	//			if (APIDGetEq(&<pid>) != eq)
	//				APIDSetEq(&<pid>, eq);
	//
	// - At any time, call APIDGetPosition(&<pid>) to retrieve the current position of the APID
	typedef struct _APID
	{
		float x;	// current position
		float eq;	// equilibrium point
		float v;	// velocity	(derivitive of x)

		float i;	// integral of x-eq
		
		float a;
		float b;
		float c;
		float reserved;
	} APID;


	// Purpose: init APID controller states
	// Params:
	//	a - Offset coefficient		(x - x0)
	//	b - Velocity coefficient	(dx/dt)
	//	c - Integral coefficient	(integral of (x-x0))
	inline void APIDInit (APID* p, float a, float b, float c)
	{
		p->a = a;
		p->b = b;
		p->c = c;
		p->x = 0.0f;
		p->eq = 0.0f;
		p->v = 0.0f;
		p->i = 0.0f;
		p->reserved = 0.0f;
	}

	// Purpose: Update APID with time step 'dt'
	inline void APIDAdvance (APID* p, float dt)
	{
		float s = p->eq - p->x;
		float dvdt;
		
		dvdt = (s*p->a - p->v*p->b + p->i*p->c);
		
		p->v += dt*dvdt;
		p->i += dt*s;
		p->x += dt*p->v;
	}


	// Purpose: Update APID with time step 'dt', but treat near-stationary as stationary.
	// APID is considered 'stationary' when |x-x0| < xthreshold and |v|<vthreshold.
	// Returns:
	//	returns 'FALSE' if stationary
	inline int APIDAdvanceSticky (APID* p, float dt,
		float xthreshold, float vthreshold)
	{
		float s = p->eq - p->x;
		float dvdt;
		
		if ( (fabsf(s) < xthreshold) && (fabsf(p->v) < vthreshold) )
		{
			p->x = p->eq;
			return 0;	// FALSE
		}

		dvdt = (s*p->a - p->v*p->b + p->i*p->c);
		
		p->v += dt*dvdt;
		p->i += dt*s;
		p->x += dt*p->v;
		
		return 1;	// TRUE
	}

	inline void APIDAdvanceClamp (APID *p, float dt, float clamp)
	{
		float s = p->eq - p->x;
		float dvdt;

		dvdt = (s*p->a - p->v*p->b + p->i*p->c);

		if (dvdt > clamp)
			dvdt = clamp;
		else if (dvdt < -clamp)
			dvdt = -clamp;
		
		p->v += dt*dvdt;
		p->i += dt*s;
		p->x += dt*p->v;
	}

	// Purpose: treat position as radians (-pi < x,eq < pi) so that
	// circular motion can be controlled
	inline void APIDCircularSetEq (APID* p, float eq)
	{
		p->eq = closeMod2pi(eq);
		p->i = 0.0f;
	}

	// Purpose: Treat position as radians (-pi < x,eq < pi) so that
	// circular motion can be controlled
	// Set equilibrium, but only reset the integrator if the new equilibrium
	// is far from the current equilibrium (difference >= threshold)
	inline void APIDCircularSetEqFrequent (APID *p, float eq, float threshold)
	{
		if (fabsf(p->eq - eq) >= threshold)
			p->i = 0.0f;

		p->eq = closeMod2pi(eq);
	}

	// Purpose: update APID with time step 'dt', but treat position as
	// radians (-pi < x,eq < pi) so that circular motion can be controlled
	inline void APIDCircularAdvance (APID* p, float dt)
	{
		float s = closeMod2pi( p->eq - p->x );
		float dvdt, x;

		dvdt = (s*p->a - p->v*p->b + p->i*p->c);

		p->v += dt*dvdt;
		p->i += dt*s;
		x = mod2pi( p->x + dt*p->v );
		p->x = x;
	}
	
	// Purpose: Update APID with time step 'dt' treating position as radians,
	// but treat near-stationary as stationary.
	// APID is considered 'stationary' when |x-x0| < xthreshold and |v|<vthreshold.
	// Returns:
	//	returns 'FALSE' if stationary
	inline int APIDCircularAdvanceSticky( APID* p, float dt,
											  float xthreshold,
											  float vthreshold)
	{
		float s = closeMod2pi( p->eq - p->x );
		float dvdt, x;
		
		if ( (fabsf(s) < xthreshold) && (fabsf(p->v) < vthreshold) )
		{
			p->x = p->eq;
			return 0;	// FALSE
		}

		dvdt = (s*p->a - p->v*p->b + p->i*p->c);
		
		p->v += dt*dvdt;
		p->i += dt*s;
		x = mod2pi( p->x + dt*p->v );
		p->x = x;
		
		return 1;	// TRUE
	}


	// Purpose: Set the equilibrium point of the APID
	inline void APIDSetEq (APID* p, float eq)
	{
		p->eq = eq;
		p->i = 0.0f;
	}

	// Purpose: Set equilibrium, but only reset the integrator if the new equilibrium
	// is far from the current equilibrium (difference >= threshold)
	inline void APIDSetEqFrequent (APID *p, float eq, float threshold)
	{
		if (fabsf(p->eq - eq) >= threshold)
			p->i = 0.0f;
			
		p->eq = eq;
	}

	// Read the equilibrium point of the APID
	inline float APIDGetEq (const APID* p)
	{
		return p->eq;
	}


	// Purpose: Set the current position of the APID
	// (normally only for initialization)
	inline void APIDSetPosition (APID* p, float x)
	{
		p->x = x;
	}

	// Purpose: Get the current position of the APID
	inline float APIDGetPosition (const APID* p)
	{
		return p->x;
	}

	// Purpose: Set the current velocity of the APID
	// (normally only for initialization or reset)
	inline void APIDSetVelocity (APID* p, float v)
	{
		p->v = v;
	}

	// Purpose: Get the current velocity of the APID
	inline float APIDGetVelocity (const APID* p)
	{
		return p->v;
	}

#ifdef __cplusplus
	}
#endif

	
	
	class Pid;

	typedef void	(Pid::*pidSimpleFn)		( float );
	typedef void	(Pid::*pidClampFn)			( float, float );
	typedef int		(Pid::*pidStickyFn)		( float, float, float );
	typedef int		(Pid::*pidClampStickyFn)	( float, float, float, float );

	class Pid : public APID
	{
		void stabilize(pidSimpleFn fn, float dtLimit, float dt);
		void stabilizeClamp(pidClampFn fn, float dtLimit, float dt, float clamp);
		int stabilizeSticky(pidStickyFn fn, float dtLimit, float dt, float xthreshold, float vthreshold);
		int stabilizeClampSticky(pidClampStickyFn fn, float dtLimit, float dt, float clamp, float xthreshold, float vthreshold);

	public:
		inline void	advanceInt			(float dt)				{ APIDAdvance (this, dt); }
		inline void	advanceClampInt		(float dt, float clamp)	{ APIDAdvanceClamp (this, dt, clamp); }
		inline int	advanceStickyInt	(float dt, float xthreshold, float vthreshold)
			{ return APIDAdvanceSticky( this, dt, xthreshold, vthreshold ); }
		int	advanceClampStickyInt	(float dt, float clamp, float xthreshold, float vthreshold);

		inline void	circularAdvanceInt(float dt) { APIDCircularAdvance(this, dt);}
		inline int	circularAdvanceStickyInt(float dt, float xthreshold, float vthreshold)
			{ return APIDCircularAdvanceSticky( this, dt, xthreshold, vthreshold );}


	public:

		//	a - Offset coefficient		(x - x0)
		//	b - Velocity coefficient	(dx/dt)
		//	c - Integral coefficient	(integral of (x-x0))
		inline void reset(float position=0.0f)
		{
			APIDSetPosition( this, position );
			APIDSetEq( this, position );
			APIDSetVelocity( this, 0.0f );
		}
		inline void reset(float position, float eq)
		{
			APIDSetPosition( this, position );
			APIDSetEq( this, eq );
			APIDSetVelocity( this, 0.0f );
		}

		// Purpose: Update APID with time step 'dt'
		inline void advance(float dt)			{ stabilize(&Pid::advanceInt, 1.0f, dt); }

		// Purpose: Update APID with time step 'dt', but treat near-stationary as stationary.
		// APID is considered 'stationary' when |x-x0| < xthreshold and |v|<vthreshold.
		// Returns:
		//	returns 'FALSE' if stationary
		inline int advanceSticky(float dt, float xthreshold, float vthreshold)
			{ return stabilizeSticky(&Pid::advanceStickyInt, 1.0f, dt, xthreshold, vthreshold); }

		inline void advanceClamp(float dt, float clamp)
			{ stabilizeClamp(&Pid::advanceClampInt, 1.0f, dt, clamp); }

		inline int advanceClampSticky(float dt, float clamp, float xthreshold, float vthreshold)
			{ return stabilizeClampSticky(&Pid::advanceClampStickyInt, 1.0f, dt, clamp, xthreshold, vthreshold); }

		// Purpose: treat position as radians (-pi < x,eq < pi) so that
		// circular motion can be controlled
		inline void circularSetEq(float eq) { APIDCircularSetEq (this, eq); }

		// Purpose: Treat position as radians (-pi < x,eq < pi) so that
		// circular motion can be controlled
		// Set equilibrium, but only reset the integrator if the new equilibrium
		// is far from the current equilibrium (difference >= threshold)
		inline void circularSetEqFrequent(float eq, float threshold)
			{ APIDCircularSetEqFrequent (this, eq, threshold); }

		// Purpose: update APID with time step 'dt', but treat position as
		// radians (-pi < x,eq < pi) so that circular motion can be controlled
		inline void circularAdvance(float dt) { stabilize(&Pid::circularAdvanceInt, 1.0f, dt); }

		// Purpose: Update APID with time step 'dt' treating position as radians,
		// but treat near-stationary as stationary.
		// APID is considered 'stationary' when |x-x0| < xthreshold and |v|<vthreshold.
		// Returns:
		//	returns 'FALSE' if stationary
		inline int circularAdvanceSticky(float dt, float xthreshold, float vthreshold)
			{ return stabilizeSticky(&Pid::circularAdvanceStickyInt, 1.0f, dt, xthreshold, vthreshold); }

		// Purpose: Set the equilibrium point of the APID
		inline void setEq(float eq) { APIDSetEq (this, eq); }

		// Purpose: Set equilibrium, but only reset the integrator if the new equilibrium
		// is far from the current equilibrium (difference >= threshold)
		inline void setEqFrequent(float eq, float threshold) { APIDSetEqFrequent (this, eq, threshold); }

		// Read the equilibrium point of the APID
		inline float getEq() const	{ return APIDGetEq (this); }

		// Purpose: Set the current position of the APID
		// (normally for initialization only)
		inline void setPosition(float x) { APIDSetPosition (this, x); }

		// Purpose: Get the current position of the APID
		inline float getPosition() const	{ return APIDGetPosition (this); }

		//	a - Offset coefficient		(x - x0)
		//	b - Velocity coefficient	(dx/dt)
		//	c - Integral coefficient	(integral of (x-x0))
		inline void init(float a, float b, float c)	{ APIDInit(this, a, b, c); }
		Pid()								{ APIDInit(this, 0.08f, 0.4f, 0.00001f); }
		Pid(float a, float b, float c)		{ APIDInit(this, a, b, c); }
		~Pid() {}

    // spring, damping, steady-state -- only changes constants, leaves integrator, position and eq unchanged
		inline void modifyCoefficients(float _a, float _b, float _c)
		{
			a = _a; b = _b; c = _c;
		}
	};

}

#endif
