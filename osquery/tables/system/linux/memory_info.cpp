/*
 *  Copyright (c) 2014-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include <string>

#include <boost/algorithm/string.hpp>

#include <osquery/core.h>
#include <osquery/tables.h>
#include <osquery/filesystem.h>

#include "osquery/core/conversions.h"

namespace osquery {
namespace tables {

const std::string kMemInfoPath = {"/proc/meminfo"};

const std::map<std::string, std::string> kMemInfoMap = {
    {"memory_total", "MemTotal:"},
    {"memory_free", "MemFree:"},
    {"buffers", "Buffers:"},
    {"cached", "Cached:"},
    {"swap_cached", "SwapCached:"},
    {"active", "Active:"},
    {"inactive", "Inactive:"},
    {"swap_total", "SwapTotal:"},
    {"swap_free", "SwapFree:"},
};

QueryData getMemoryInfo(QueryContext& context) {
  QueryData results;
  Row r;

  std::string meminfo_content;
  if (forensicReadFile(kMemInfoPath, meminfo_content).ok()) {
    // Able to read meminfo file, now grab info we want
    for (const auto& line : split(meminfo_content, "\n")) {
      std::vector<std::string> tokens;
      boost::split(
          tokens, line, boost::is_any_of("\t "), boost::token_compress_on);
      // Look for mapping
      for (const auto& singleMap : kMemInfoMap) {
        if (line.find(singleMap.second) == 0) {
          long value = 0;
          if (safeStrtol(tokens[1], 10, value)) {
            r[singleMap.first] = INTEGER(value * 1024l);
          }
          break;
        }
      }
    }
  }
  results.push_back(r);
  return results;
}
}
}
