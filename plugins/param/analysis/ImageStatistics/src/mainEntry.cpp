#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 1

#include <tuttle/plugin/Plugin.hpp>
#include "ImageStatisticsPluginFactory.hpp"

namespace OFX {
namespace Plugin {

void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	mAppendPluginFactory( ids, tuttle::plugin::imageStatistics::ImageStatisticsPluginFactory, "tuttle.imagestatistics" );
}

}
}
