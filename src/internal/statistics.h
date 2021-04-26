#ifndef STEVESCH_MATH_INTERNAL_STATISTICS_H_
#define STEVESCH_MATH_INTERNAL_STATISTICS_H_

#include "scalar.h"
#include <vector>

namespace stevesch
{
	class RateAccumulator
	{
		float	mfRate;			// average count per unit of time
		float	mfAccum;			// accumulated count from previous updates

	public:
		RateAccumulator( float fRate=1.0f ) : mfRate(fRate), mfAccum(0.0f)	{}

		void setRate(float fRate)	{ mfRate = fRate; }
		float getRate() const		{ return mfRate; }
		void clear()				{ mfAccum = 0.0f; }

		void putBack(int n)			{ mfAccum += n; }

		int update( float fTime )
		{
			int n = 0;

			mfAccum += fTime*mfRate;
			if (mfAccum > 1.0f)
			{
				n = ftoi(mfAccum);
				mfAccum -= n;
			}
			return n;
		}
	};

	////////////////////////////////////////////////////////////////////////
	
	template <typename T>
	class ProbabilityTable
	{
		struct Weight
		{
			float	mfWeightAccum;	// sum of normalized weight and all previous normalized weights
			float	mfWeight;
			T		mData;

			Weight( float fWeight, const T& crData ) : mfWeightAccum(0.0f),
				mfWeight(fWeight), mData(crData)
			{
			}
		};

		std::vector<Weight>	mTable;

	public:
		// callbacks should return 'false' to terminate traversal
		typedef bool (*TRAVERSAL_CALLBACK)(float fWeight, T& rData, void* pCallbackContext);

		void insert( float fWeight, const T& crData );
		const T* get( float fValue ) const;	// get data given a probability value in range [0.0, 1.0]
		const T* getRandom( RandGen& r=S_RandGen ) const	{ return get( r.getFloat() ); }	// use uniform random distribution
		void clear();	// clear table

		bool traverse(TRAVERSAL_CALLBACK pCallback, void* pCallbackContext);	// calls the specified callback for each
																				// element currently in the table.
																				// returns false if callback terminated
																				// traversal prematurely (by returning false)
	};

	
	template <typename T>
	bool ProbabilityTable<T>::traverse(TRAVERSAL_CALLBACK pCallback, void* pCallbackContext)
	{
		uint32_t i;
		for (i=0; i<mTable.size(); i++)
		{
			if (!pCallback( mTable[i].mfWeight, mTable[i].mData, pCallbackContext ))
				return false;
		}
		return true;
	}


	template <typename T>
	void ProbabilityTable<T>::insert( float fWeight, const T& crData )
	{
		float fWeightSum = 0.0f;
		float fWeightAccum = 0.0f;
		float fInvWeightSum = 1.0f;
		uint32_t i;

    // mTable.push_back( Weight( fWeight, crData ) );
		mTable.emplace_back( fWeight, crData );
		
		const uint32_t cnTableSize = mTable.size();
		for (i=0; i<cnTableSize; i++)
		{
			fWeightSum += mTable[i].mfWeight;
		}
		
		if (fWeightSum > 0.0f)
		{
			fInvWeightSum = recipf( fWeightSum );
		}

		for (i=0; i<cnTableSize; i++)
		{
			fWeightAccum += mTable[i].mfWeight * fInvWeightSum;
			mTable[i].mfWeightAccum = fWeightAccum;
		}

		// overkill (push past 1.0 to guarantee floating-point error doesn't interfere)
		mTable[cnTableSize-1].mfWeightAccum = 1.0e+6f;
	}

	// get data given a probability value in range [0.0, 1.0]
	template <typename T>
	const T* ProbabilityTable<T>::get( float fValue ) const
	{
		const uint32_t cnTableSize = mTable.size();
		uint32_t i;
		for (i=0; i<cnTableSize; i++)
		{
			if ( fValue <= mTable[i].mfWeightAccum )
				return &mTable[i].mData;
		}

		return NULL;	// fail (table is empty or fValue is out of range [0.0, 1.0]
	}


	// clear table
	template <typename T>
	void ProbabilityTable<T>::clear()
	{
		mTable.clear();
	}

	////////////////////////////////////////////////////////////////////////
}

#endif
