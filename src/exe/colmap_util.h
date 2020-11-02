#ifndef COLMAP_SRC_COLMAP_UTIL_H_
#define COLMAP_SRC_COLMAP_UTIL_H_
#endif // !COLMAP_SRC_COLMAP_UTIL_H_

#include <vector>
#include "feature/types.h"

namespace colmap {
	int RunSimpleFeatureExtractor(int argc, char** argv, std::vector<FeatureDescriptors>& query);
}