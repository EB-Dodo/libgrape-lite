/** Copyright 2020 Alibaba Group Holding Limited.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef EXAMPLES_ANALYTICAL_APPS_SSSP_SSSP_CONTEXT_H_
#define EXAMPLES_ANALYTICAL_APPS_SSSP_SSSP_CONTEXT_H_

#include <iomanip>
#include <iostream>
#include <limits>

#include <grape/grape.h>

namespace grape {

/**
 * @brief Context for the parallel version of SSSP.
 *
 * @tparam FRAG_T
 */
template <typename FRAG_T>
class SSSPContext : public ContextBase<FRAG_T> {
 public:
  using oid_t = typename FRAG_T::oid_t;
  using vid_t = typename FRAG_T::vid_t;

  void Init(const FRAG_T& frag, ParallelMessageManager& messages,
            oid_t source_id) {
    this->source_id = source_id;
    auto vertices = frag.Vertices();
    partial_result.Init(vertices, std::numeric_limits<double>::max());

    curr_modified.init(frag.GetVerticesNum());
    next_modified.init(frag.GetVerticesNum());

#ifdef PROFILING
    preprocess_time = 0;
    exec_time = 0;
    postprocess_time = 0;
#endif
  }

  void Output(const FRAG_T& frag, std::ostream& os) {
    // If the distance is the max value for vertex_data_type
    // then the vertex is not connected to the source vertex.
    // According to specs, the output should be +inf
    auto inner_vertices = frag.InnerVertices();
    for (auto v : inner_vertices) {
      double d = partial_result[v];
      if (d == std::numeric_limits<double>::max()) {
        os << frag.GetId(v) << " infinity" << std::endl;
      } else {
        os << frag.GetId(v) << " " << std::scientific << std::setprecision(15)
           << d << std::endl;
      }
    }
#ifdef PROFILING
    VLOG(2) << "preprocess_time: " << preprocess_time << "s.";
    VLOG(2) << "exec_time: " << exec_time << "s.";
    VLOG(2) << "postprocess_time: " << postprocess_time << "s.";
#endif
  }

  oid_t source_id;
  VertexArray<double, vid_t> partial_result;

  Bitset curr_modified, next_modified;

#ifdef PROFILING
  double preprocess_time = 0;
  double exec_time = 0;
  double postprocess_time = 0;
#endif
};
}  // namespace grape

#endif  // EXAMPLES_ANALYTICAL_APPS_SSSP_SSSP_CONTEXT_H_
