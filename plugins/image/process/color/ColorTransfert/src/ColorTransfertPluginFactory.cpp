#include "ColorTransfertPluginFactory.hpp"
#include "ColorTransfertPlugin.hpp"
#include "ColorTransfertDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <limits>

namespace tuttle {
namespace plugin {
namespace colorTransfert {

static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ColorTransfertPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels(
		"TuttleColorTransfert",
		"ColorTransfert",
		"ColorTransfert" );
	desc.setPluginGrouping( "tuttle/image/process/color" );

	desc.setDescription(
			"ColorTransfert\n"
			"\n"
			"The standard usage of this node is to impose one image's color characteristics to another.\n"
			"If srcRef input clip is not the same as the source clip, "
			"it applies the tranformation from srcRef to dstRef on the source clip.\n"
			"\n"
			"Warning: pixel values <= 0.0001 will be clamp, because these values "
			"are undefined in the colorspace used for the transformation.\n"
			"\n"
			"Technically, the node modify the statistics of the image in a particular colorspace lambda-alpha-beta. "
			"This colorspace, created by Rederman and al., is closed to the human perception and minimizes "
			"the correlation between channels for many natural scenes.\n"
			"For more details see the article:\n"
			"'Color Transfert between images' by Erik Reinhard, Michael Ashikhmin, Bruce Gooch and Peter Shirley. University of Utah.\n"
		);

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );
	desc.setRenderThreadSafety( OFX::eRenderInstanceSafe );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ColorTransfertPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
													 OFX::EContext context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	// srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	// dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::ClipDescriptor* srcRefClip = desc.defineClip( kClipSrcRef );
	srcRefClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcRefClip->addSupportedComponent( OFX::ePixelComponentRGB );
	// srcRefClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcRefClip->setSupportsTiles( kSupportTiles );
	srcRefClip->setOptional( true );

	OFX::ClipDescriptor* dstRefClip = desc.defineClip( kClipDstRef );
	dstRefClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstRefClip->addSupportedComponent( OFX::ePixelComponentRGB );
//	dstRefClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstRefClip->setSupportsTiles( kSupportTiles );

	OFX::DoubleParamDescriptor* averageCoef = desc.defineDoubleParam( kParamAverageCoef );
	averageCoef->setLabel( "Average color coef" );
	averageCoef->setDisplayRange( 0.0, 1.0 );
	averageCoef->setDefault( 0.8 );
	averageCoef->setHint(
			"Percentage of correction of the average color.\n"
			"It is often advantageous to reduce this ratio, "
			"especially if the two images are very different. "
			"Typical values are between 0.5 and 0.8."
		);

	OFX::DoubleParamDescriptor* dynamicCoef = desc.defineDoubleParam( kParamDynamicCoef );
	dynamicCoef->setLabel( "Dynamic coef" );
	dynamicCoef->setDisplayRange( 0.0, 1.0 );
	dynamicCoef->setDefault( 1.0 );
	dynamicCoef->setHint(
			"Percentage of correction of the standard deviation."
		);

	// output region
	OFX::Double2DParamDescriptor* regionA = desc.defineDouble2DParam( kParamRegionA );
	regionA->setLabel( "Region" );
	regionA->setDefault( -0.5, -0.5 );
	regionA->setIsSecret( true ); ///< @todo
	OFX::Double2DParamDescriptor* regionB = desc.defineDouble2DParam( kParamRegionB );
	regionB->setLabel( "" );
	regionB->setDefault( 0.5, 0.5 );
	regionB->setIsSecret( true ); ///< @todo

	// same region
	OFX::BooleanParamDescriptor* sameRegion = desc.defineBooleanParam( kParamSameRegion );
	sameRegion->setDefault( true );
	sameRegion->setIsSecret( true ); ///< @todo

	// input region
	OFX::Double2DParamDescriptor* inputRegionA = desc.defineDouble2DParam( kParamInputRegionA );
	inputRegionA->setLabel( "Input region" );
	inputRegionA->setDefault( -0.5, -0.5 );
	inputRegionA->setIsSecret( true ); ///< @todo
	OFX::Double2DParamDescriptor* inputRegionB = desc.defineDouble2DParam( kParamInputRegionB );
	inputRegionB->setLabel( "" );
	inputRegionB->setDefault( 0.5, 0.5 );
	inputRegionB->setIsSecret( true ); ///< @todo
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ColorTransfertPluginFactory::createInstance( OfxImageEffectHandle handle,
															   OFX::EContext context )
{
	return new ColorTransfertPlugin( handle );
}

}
}
}

