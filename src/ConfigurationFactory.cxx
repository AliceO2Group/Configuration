/*
 * ConfigurationDispatcher.cxx
 *
 *  Created on: Mar 7, 2016
 *      Author: pboescho
 */

#include <stdexcept>
#include <boost/algorithm/string.hpp>
#include <Configuration/ConfigurationFactory.h>
#include <Configuration/UriParser/UriParser.hpp>
#include <Configuration/EtcdConfiguration.h>
#include <Configuration/FileConfiguration.h>

std::unique_ptr<ConfigurationInterface> ConfigurationFactory::getConfiguration(std::string uri)
{
  http::url parsed = http::ParseHttpUrl(uri);

  if (parsed.protocol == "file") {
    // If the "authority" part of the URI is missing (host, port, etc), the parser
    // will consider the thing before the first delimiter ('/') of the path as authority,
    // so we have to include that in the path we use.
    auto path = parsed.host + parsed.path;
    return std::unique_ptr<FileConfiguration>(new FileConfiguration(path));
  } else if (parsed.protocol == "etcd") {
    return std::unique_ptr<EtcdConfiguration>(new EtcdConfiguration(parsed.host, parsed.port));
  } else {
    throw std::runtime_error("Unrecognized URI scheme");
  }
}

