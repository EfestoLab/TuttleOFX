#ifndef _TERRY_SAMPLER_DETAILS_HPP_
#define _TERRY_SAMPLER_DETAILS_HPP_

#include <terry/numeric/pixel_numeric_operations.hpp>

#include "sampler.hpp"

#include <terry/clamp.hpp>
#include <terry/globals.hpp>
#include <terry/basic_colors.hpp>

#include <cmath>
#include <vector>


namespace terry {
using namespace boost::gil;
namespace sampler {

namespace details {


template <typename Weight>
struct add_dst_mul_src_channel
{
	const Weight _w;

	add_dst_mul_src_channel( const Weight w ) : _w( w ) { }

	template <typename SrcChannel, typename DstChannel>
	void operator( )( const SrcChannel& src, DstChannel & dst ) const
	{
		dst += DstChannel( src * _w );
	}
};

template <typename SrcP, typename Weight, typename DstP>
struct add_dst_mul_src
{
	void operator( )( const SrcP& src, const Weight weight, DstP & dst ) const
	{
		static_for_each( src, dst, add_dst_mul_src_channel<Weight > ( weight ) );
	}
};


/**
 * @brief Get pixels around a particular position.
 * @param[in] loc locator which points to a pixel
 * @param[in] pt0 x,y position of loc
 * @param[in] windowWidth the region inside which we search our pixels
 * @param[out] ptN pixel value to retrieve
 *
 * it's to use with (B,C) filter
 * number of points need to be even
 *
 * -----------------
 * | A | B | C | D |
 * -----------------
 *       ^..... loc is pointing to D point
 */
template < typename xy_locator, typename SrcP >
void getPixelsPointers( const xy_locator& loc, const point2<std::ptrdiff_t>& p0, const int& windowWidth, const int& imageWidth, const EParamFilterOutOfImage& outOfImageProcess, std::vector< SrcP >& src )
{
	int middlePosition = floor( (src.size() - 1) * 0.5 );

	if( p0.x < 0 )
	{
		switch( outOfImageProcess )
		{
			case eParamFilterOutBlack :
			{
				src.at( middlePosition ) = get_black<SrcP>();
				break;
			}
			case eParamFilterOutTransparency :
			{
				src.at( middlePosition ) = SrcP(0);
				break;
			}
			case eParamFilterOutCopy :
			{
				src.at( middlePosition ) = loc.x()[ - p0.x ];
				break;
			}
			case eParamFilterOutMirror :
			{
				src.at( middlePosition ) = SrcP(0);
				break;
			}
		}
	}
	else
	{
		if( p0.x > imageWidth - 1 )
		{
			switch( outOfImageProcess )
			{
				case eParamFilterOutBlack :
				{
					src.at( middlePosition ) = get_black<SrcP>();
					break;
				}
				case eParamFilterOutTransparency :
				{
					src.at( middlePosition ) = SrcP(0);
					break;
				}
				case eParamFilterOutCopy :
				{
					src.at( middlePosition ) = loc.x()[ imageWidth - 1 - p0.x ];
					break;
				}
				case eParamFilterOutMirror :
				{
					src.at( middlePosition ) = SrcP(0);
					break;
				}
			}
		}
		else
		{
			src.at( middlePosition ) = *loc;
		}
	}

	// from center to left
	for( int i = middlePosition - 1; i > -1; i-- )
	{
		if( ( p0.x - (middlePosition - i) > -1 ) )
		{
			if( ( p0.x - (middlePosition - i) < imageWidth ) )
			{
				src.at( i ) = loc.x( )[ - (middlePosition - i) ];
			}
			else
			{
				switch( outOfImageProcess )
				{
					case eParamFilterOutBlack :
					{
						src.at( i ) = get_black<SrcP>();
						break;
					}
					case eParamFilterOutTransparency :
					{
						src.at( i ) = SrcP(0);
						break;
					}
					case eParamFilterOutCopy :
					{
						src.at( i ) = src.at( i + 1 );
						break;
					}
					case eParamFilterOutMirror :
					{
						src.at( i ) = SrcP(0);
						break;
					}
				};
			}
		}
		else
		{
			switch( outOfImageProcess )
			{
				case eParamFilterOutBlack :
				{
					src.at( i ) = get_black<SrcP>();
					break;
				}
				case eParamFilterOutTransparency :
				{
					src.at( i ) = SrcP(0);
					break;
				}
				case eParamFilterOutCopy :
				{
					src.at( i ) = src.at( i + 1 );
					break;
				}
				case eParamFilterOutMirror :
				{
					src.at( i ) = SrcP(0);
					break;
				}
			};
		}
	}

	// from center to right
	for( int i = middlePosition + 1; i < (int) src.size(); i++ )
	{
		if( ( p0.x - (middlePosition - i) < imageWidth ) )
		{
			if( p0.x - (middlePosition - i) < 0 )
			{
				switch( outOfImageProcess )
				{
					case eParamFilterOutBlack :
					{
						src.at( i ) = get_black<SrcP>();
						break;
					}
					case eParamFilterOutTransparency :
					{
						src.at( i ) = SrcP(0);
						break;
					}
					case eParamFilterOutCopy :
					{
						src.at( i ) = loc.x( )[ - p0.x ];
						break;
					}
					case eParamFilterOutMirror :
					{
						src.at( i ) = SrcP(0);
						break;
					}
				};
			}
			else
			{
				src.at( i ) = loc.x( )[  - (middlePosition - i) ];
			}
		}
		else
		{
			switch( outOfImageProcess )
			{
				case eParamFilterOutBlack :
				{
					src.at( i ) = get_black<SrcP>();
					break;
				}
				case eParamFilterOutTransparency :
				{
					src.at( i ) = SrcP(0);
					break;
				}
				case eParamFilterOutCopy :
				{
					src.at( i ) = loc.x()[ imageWidth - 1 - p0.x ];
					break;
				}
				case eParamFilterOutMirror :
				{
					src.at( i ) = SrcP(0);
					break;
				}
			};
		}
	}

}

template <typename SrcP, typename F, typename DstP>
struct process1Dresampling
{
	void operator( )( const std::vector<SrcP>& src, const std::vector<F>& weight, DstP& dst ) const
	{
		DstP mp( 0 );
		for( size_t i = 0; i < src.size(); i++ )
			details::add_dst_mul_src<SrcP, float, DstP > ( )( src.at(i), weight.at(i) , mp );
		dst = mp;
	}
};

/// @todo specialization for SIMD
//template <typename F>
//struct process1Dresampling
//{
//	void operator()( const std::vector<rgba32f_t> src, const std::vector<F> weight, rgba32f_t & dst ) const
//	{
//		//...
//	}
//};

template <typename DstP, typename SrcView, typename Sampler, typename F>
bool process2Dresampling( Sampler& sampler, const SrcView& src, const point2<F>& p, const std::vector<double>& xWeights, const std::vector<double>& yWeights, const size_t& windowSize, const EParamFilterOutOfImage& outOfImageProcess, typename SrcView::xy_locator& loc, DstP& result )
{
	typedef typename SrcView::value_type SrcP;

	typedef typename floating_pixel_from_view<SrcView>::type SrcC; //PixelFloat;

	point2<std::ptrdiff_t> pTL( ifloor( p ) ); // the closest integer coordinate top left from p

	SrcC mp( 0 );

	std::vector < SrcP > ptr;
	std::vector < SrcC > xProcessed;

	ptr.assign       ( windowSize, SrcP(0) );
	xProcessed.assign( windowSize, SrcC(0) );

	size_t middlePosition = floor((windowSize - 1) * 0.5);


	// first process the middle point
	// if it's mirrored, we need to copy the center point
	if( (pTL.y < 0) )
	{
		switch( outOfImageProcess )
		{
			case eParamFilterOutBlack :
			{
				xProcessed.at( middlePosition ) = get_black<DstP>();
			break;
			}
			case eParamFilterOutTransparency :
			{
				xProcessed.at( middlePosition ) = SrcP(0);
				break;
			}
			case eParamFilterOutCopy :
			{
				loc.y( ) -= pTL.y;
				getPixelsPointers( loc, pTL, windowSize, src.width(), outOfImageProcess, ptr );
				process1Dresampling<SrcP, F, SrcC> () ( ptr, xWeights, xProcessed.at( middlePosition ) );
				loc.y( ) += pTL.y;
				break;
			}
			case eParamFilterOutMirror :
			{
				xProcessed.at( middlePosition ) = SrcP(1);
				break;
			}
		}
	}
	else if( (pTL.y > (int) ( src.height( ) - 1.0 ) ) ) // upper the image
	{
		//TUTTLE_COUT( src.height() << " @@ " << (pTL.y - src.height() ) );
		switch( outOfImageProcess )
		{
			case eParamFilterOutBlack :
			{
				xProcessed.at( middlePosition ) = get_black<DstP>();
				break;
			}
			case eParamFilterOutTransparency :
			{
				xProcessed.at( middlePosition ) = SrcP(0);
				break;
			}
			case eParamFilterOutCopy :
			{
				loc.y( ) -= pTL.y - src.height() + 1.0 ;
				getPixelsPointers( loc, pTL, windowSize, src.width(), outOfImageProcess, ptr );
				process1Dresampling<SrcP, F, SrcC> () ( ptr, xWeights, xProcessed.at( middlePosition ) );
				loc.y( ) += pTL.y - src.height() + 1.0;
				break;
			}
			case eParamFilterOutMirror :
			{
				xProcessed.at( middlePosition ) = SrcP(1);
				break;
			}
		}
	}
	else
	{
		getPixelsPointers( loc, pTL, windowSize, src.width() , outOfImageProcess, ptr );
		process1Dresampling<SrcP, F, SrcC> () ( ptr, xWeights, xProcessed.at( middlePosition ) );
	}

	// from center to bottom
	for( int i = middlePosition - 1; i > -1; i-- )
	{
		if( (int) ( pTL.y - (middlePosition - i) ) < (int) src.height( ) )
		{
			if( (int) ( pTL.y - (middlePosition - i) ) < 0 )
			{
				switch( outOfImageProcess )
				{
					case eParamFilterOutBlack :
					{
						xProcessed.at( i ) = get_black<DstP>();
						break;
					}
					case eParamFilterOutTransparency :
					{
						xProcessed.at( i ) = SrcP(0);
						break;
					}
					case eParamFilterOutCopy :
					{
						xProcessed.at( i ) = xProcessed.at( i + 1 );
						break;
					}
					case eParamFilterOutMirror :
					{
						xProcessed.at( i ) = xProcessed.at( i + 1 );
						break;
					}
				}
			}
			else
			{
				loc.y( ) -= (middlePosition - i);
				getPixelsPointers( loc, pTL, windowSize, src.width(), outOfImageProcess, ptr );
				process1Dresampling<SrcP, F, SrcC> () ( ptr, xWeights, xProcessed.at( i ) );
				loc.y( ) += (middlePosition - i);
			}
		}
		else
		{
			switch( outOfImageProcess )
			{
				case eParamFilterOutBlack :
				{
					xProcessed.at( i ) = get_black<DstP>();
					break;
				}
				case eParamFilterOutTransparency :
				{
					xProcessed.at( i ) = SrcP(0);
					break;
				}
				case eParamFilterOutCopy :
				{
					xProcessed.at( i ) = xProcessed.at( i + 1 );
					break;
				}
				case eParamFilterOutMirror :
				{
					xProcessed.at( i ) = xProcessed.at( i + 1 );
					break;
				}
			}
		}
	}

	// from center to top
	for( int i = middlePosition + 1; i < (int)windowSize; i++ )
	{
		if( (int) ( pTL.y + (i - middlePosition) ) < (int) src.height( ) )
		{
			if( (int) ( pTL.y + (i - middlePosition) )  < 0 )
			{
				xProcessed.at( i ) = xProcessed.at( i - 1 );
			}
			else
			{
				loc.y( ) -= ( middlePosition - i );
				getPixelsPointers( loc, pTL, windowSize, src.width(), outOfImageProcess, ptr );
				process1Dresampling<SrcP, F, SrcC> () ( ptr, xWeights, xProcessed.at( i ) );
				loc.y( ) += ( middlePosition - i );
			}
		}
		else
		{
			switch( outOfImageProcess )
			{
				case eParamFilterOutBlack :
				{
					xProcessed.at( i ) = get_black<DstP>();
					break;
				}
				case eParamFilterOutTransparency :
				{
					xProcessed.at( i ) = SrcP(0);
					break;
				}
				case eParamFilterOutCopy :
				{
					xProcessed.at( i ) = xProcessed.at( i - 1 );
					break;
				}
				case eParamFilterOutMirror :
				{
					xProcessed.at( i ) = xProcessed.at( i - 1 );
					break;
				}
			}
		}
	}


	// vertical process
	process1Dresampling<SrcC, F, SrcC> () ( xProcessed, yWeights, mp );

	// result is rgba8
	//proc( mp );
	// Convert from floating point average value to the source type
	DstP src_result;
	//cast_pixel  ( mp, src_result );
	color_convert( mp, result );

	return true;
}


}
}
}

#endif