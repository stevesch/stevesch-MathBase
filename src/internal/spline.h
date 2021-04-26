#ifndef STEVESCH_MATH_INTERNAL_SPLINE_H_
#define STEVESCH_MATH_INTERNAL_SPLINE_H_

namespace stevesch
{
  template <int N>
  void set(float vout[N], const float v[N]) {
    for (int i=0; i<N; ++i) { vout[i] = v[i]; }
  }

  template <int N>
  void addScaled(float vout[N], const float v1[N], float s1, const float v2[N], float s2) {
    float tmp[3];
    for (int i=0; i<N; ++i) {
      tmp[i] = s1 * v1[i] + s2 * v2[i];
    }
    vout = tmp;
  }

  template <int N>
  void sub(float vout[N], const float v1[N], const float v2[N]) {
    float tmp[N];
    for (int i=0; i<N; ++i) {
      tmp[i] = v1[i] - v2[i];
    }
    vout = tmp;
  }

  template <int N>
  void scale(float vout[N], const float v[N], float s) {
    float tmp[N];
    for (int i=0; i<N; ++i) {
      tmp[i] = s * v[i];
    }
    vout = tmp;
  }

	//-----------------------------------------------------------------------------
	// Name: T_EvaluateCatmullRom()
	// Desc: Evaluate a cubic parametric equation. Returns the point at u on a
	//       Catmull-Rom curve.
	//-----------------------------------------------------------------------------
	template <typename T_VECTOR>
	void T_EvaluateCatmullRom( T_VECTOR& vDst,
							   const T_VECTOR& p1, const T_VECTOR& p2, 
							   const T_VECTOR& p3, const T_VECTOR& p4,
							   float u )
	{
		// Generate coefficients from four spline points
//		T_VECTOR a =   -p1 + 3*p2 - 3*p3 + p4;
//		T_VECTOR b =  2*p1 - 5*p2 + 4*p3 - p4;
//		T_VECTOR c =   -p1        +   p3;
//		T_VECTOR d =         2*p2;
		T_VECTOR a, b, c, d;
		addScaled( a, p2, 3.0f, p3, -3.0f );	// 3*p2 - 3*p3
		a += p4;
		a -= p1;

		addScaled( b, p2, -5.0f, p3, 4.0f );	// -5*p2 + 4*p3
		addScaled( b, b, p1, 2.0f );			// ...+ 2*p1
		b -= p4;										// -p4

		sub( c, p3, p1 );			// -p1 + p3

		scale( d, p2, 2.0f );		// 2*p2

		// Evaluate the equation at u, where:
		//    f(u) = 0.5 * ( au^3 + bu^2 + cu + d )
		// return 0.5f * ( ( ( a * u + b ) * u + c ) * u + d );

		addScaled( vDst, b, a, u );		// (a*u + b)
		addScaled( vDst, c, vDst, u );	// (...)*u + c
		addScaled( vDst, d, vDst, u );	// (...)*u + d
		vDst *= 0.5f;
	}

	//-----------------------------------------------------------------------------
	// Name: T_GetSplinePoint()
	// Desc: Returns a point on a spline. The spline is defined by an array of
	//       points, and the point and tangent returned are located at position t
	//       on the spline, where 0 < t < nNumSpinePts.
	//-----------------------------------------------------------------------------
	template <typename T_VECTOR>
	void T_GetSplinePoint( const T_VECTOR* pSpline, int nNumSpinePts,
								float t,
								T_VECTOR* pvPoint,		// out
								T_VECTOR* pvTangent=0 )	// out
	{
		int p0 = ( t > 1.0 ) ? (int)floorf(t)-1 : nNumSpinePts-1;
		int p1 = ( p0 < nNumSpinePts-1 ) ? p0 + 1 : 0;
		int p2 = ( p1 < nNumSpinePts-1 ) ? p1 + 1 : 0;
		int p3 = ( p2 < nNumSpinePts-1 ) ? p2 + 1 : 0;
		float u  = t - floorf(t);

		if( pvPoint )
		{
			T_EvaluateCatmullRom( *pvPoint,
								   pSpline[p0], pSpline[p1], 
								   pSpline[p2], pSpline[p3], u );
		}

		if( pvTangent )
		{
			T_VECTOR v0;
      T_VECTOR v1;
			sub( v0, pSpline[p2] - pSpline[p0] );
			sub( v1, pSpline[p3] - pSpline[p1] );
			float hu = 0.5f*u;
			addScaled( *pvTangent, v0, (0.5f - hu),
											 v1, hu );
//			(*pvTangent) = 0.5f * ( (1-u) * ( pSpline[p2] - pSpline[p0] ) + 
//									  (u) * ( pSpline[p3] - pSpline[p1] ) );
		}
	}

/*
	controls are p1, t1, p2, t2, with tangents (t1 and t2) forward-facing on the curve.
    bezier path, given by
    v(t)  = (1-t)^3 * p1 + 3*t*(1-t)^2*(p1+t1) + 3*t^2*(1-t)*(p2-t2) + t^3*p2; t in [0,1]

	uuu*p1 + 3*t*uu*p1 + 3*t*uu*t1 + 
	3*tt*u*p2 - 3*tt*u*t2 + ttt*p2

	==
	p1*(uuu + 3*t*uu) +
	p2*(ttt + 3*tt*u) +
	t1*(3*t*uu) -
	t2*(3*tt*u)
	== p1*(uuu + ca) + p2*(ttt - cb) + t1*ca + t2*cb
	ca := 3*t*uu
	cb := -3*tt*u

    and the length is 
    Lw = /int_{0}^{1} |w'(t)| dt
*/

	// controls are p1, t1, p2, t2, with tangents (t1 and t2) forward-facing on the curve
	template <typename T_VECTOR>
	void T_GetBezierPoint( T_VECTOR& vPoint,
						   const T_VECTOR& rPoint0, const T_VECTOR& rTan0,
						   const T_VECTOR& rPoint1, const T_VECTOR& rTan1,
						   float t )
	{
		float u = 1-t;
		float tt = t*t;
		float uu = u*u;
/*
		float ttt = tt*t;
		float uuu = uu*u;
		T_VECTOR vt1;
		add( vt1, rPoint0, rTan0 );					// p1 + t1
		T_VECTOR::Sub( vPoint, rPoint1, rTan1 );				// p2 - t2
		addScaled( vPoint, vt1, 3.0f*t*uu, vPoint, 3.0f*tt*u );	// 3*t*(1-t)^2*(p1+t1) + 3*t^2*(1-t)*(p2-t2)
		addScaled( vPoint, vPoint, rPoint0, uuu );			// ... + (1-t)^3*p1
		addScaled( vPoint, vPoint, rPoint1, ttt );			// ... + t^3*p2
*/
		float ca = 3.0f*t*uu;
		float cb = -3.0f*tt*u;
		addScaled( vPoint, rPoint0, (u*uu + ca), rPoint1, (t*tt - cb) );
		addScaled( vPoint, vPoint, rTan0, ca );
		addScaled( vPoint, vPoint, rTan1, cb );
	}
}

#endif
