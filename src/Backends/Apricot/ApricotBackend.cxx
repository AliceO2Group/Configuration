// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file ApricotBackend.h
/// \brief Configuration interface to the Apricot key-value store
///
/// \author Pascal Boeschoten, CERN

#include "ApricotBackend.h"
#include <boost/property_tree/json_parser.hpp>

namespace o2
{
namespace configuration
{
namespace backends
{

std::size_t WriteData(const char* in, std::size_t size, std::size_t num, std::string* out) {
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
};

ApricotBackend::ApricotBackend(const std::string& host, int port) :
    mUrl(host + ":" + std::to_string(port))
{
  mCurl = curl_easy_init();
  curl_easy_setopt(mCurl, CURLOPT_SSL_VERIFYPEER, 0); 
  curl_easy_setopt(mCurl, CURLOPT_CONNECTTIMEOUT, 3);
  curl_easy_setopt(mCurl, CURLOPT_TIMEOUT, 3);
}

ApricotBackend::~ApricotBackend()
{
  curl_easy_cleanup(mCurl);
  curl_global_cleanup();
}

auto ApricotBackend::replaceDefaultWithSlash(const std::string& path) -> std::string
{
  auto p = path;
  std::replace(p.begin(), p.end(), getSeparator(), '/');
  return p;
}

void ApricotBackend::putString(const std::string& /*path*/, const std::string& /*value*/)
{
  throw std::runtime_error("Apricot backend does not support putting values");
}

boost::optional<std::string> ApricotBackend::getString(const std::string& path)
{
  return get(path);
}


std::string ApricotBackend::get(const std::string& path) {
  std::string response;
  std::string url = mUrl + "/" + replaceDefaultWithSlash(addApricotPrefix(path)) +  mQueryParams;
  CURLcode res;
  long responseCode;
  curl_easy_setopt(mCurl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, WriteData);
  curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, &response);

  res = curl_easy_perform(mCurl);
  curl_easy_getinfo(mCurl, CURLINFO_RESPONSE_CODE, &responseCode);

  if (res != CURLE_OK) {
    throw std::runtime_error(std::string(curl_easy_strerror(res)) + " " + url);
    return {}; 
  }
  if (responseCode < 200 || responseCode > 206) {
    throw std::runtime_error("Wrong status code: " + std::to_string(responseCode));
  }
  return response;
}

boost::property_tree::ptree ApricotBackend::getRecursive(const std::string& path)
{
  std::istringstream ss; 
  ss.str(get(path));
  boost::property_tree::ptree tree;
  boost::property_tree::read_json(ss, tree);
  return tree;
}

KeyValueMap ApricotBackend::getRecursiveMap(const std::string& path)
{
  KeyValueMap map;
  auto subTree = getRecursive(path);

  // define lambda to recursively interate tree
  using boost::property_tree::ptree;
  std::function<void(const ptree&, std::string)> parse = [&](const ptree& node, std::string key) {
    map[key] = std::move(node.data());
    key = key.empty() ? "" : key + getSeparator();
    for (auto const &it: node) {
      parse(it.second, key + it.first);
    }   
  };  
  parse(subTree, std::string());
  return map;
}

} // namespace backends
} // namespace configuration
} // namespace o2
