/*
 * Copyright (c) 2019-2021, NVIDIA CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <raft/cudart_utils.h>
#include <iostream>
#include <raft/cuda_utils.cuh>
#include <raft/label/classlabels.cuh>
#include <raft/spatial/knn/knn.hpp>
#include <random/make_blobs.cuh>
#include <rmm/device_uvector.hpp>
#include <selection/knn.cuh>
#include <vector>
#include "test_utils.h"

namespace MLCommon {
namespace Selection {

struct KNNClassifyInputs {
  int rows;
  int cols;
  int n_labels;
  float cluster_std;
  int k;
};

class KNNClassifyTest : public ::testing::TestWithParam<KNNClassifyInputs> {
 protected:
  void basicTest()
  {
    raft::handle_t handle;
    cudaStream_t stream = handle.get_stream();

    params = ::testing::TestWithParam<KNNClassifyInputs>::GetParam();

    raft::allocate(train_samples, params.rows * params.cols, stream);
    raft::allocate(train_labels, params.rows, stream);

    raft::allocate(pred_labels, params.rows, stream);

    raft::allocate(knn_indices, params.rows * params.k, stream);
    raft::allocate(knn_dists, params.rows * params.k, stream);

    MLCommon::Random::make_blobs<float, int>(train_samples,
                                             train_labels,
                                             params.rows,
                                             params.cols,
                                             params.n_labels,
                                             stream,
                                             true,
                                             nullptr,
                                             nullptr,
                                             params.cluster_std);

    rmm::device_uvector<int> unique_labels(0, stream);
    auto n_classes = raft::label::getUniquelabels(unique_labels, train_labels, params.rows, stream);

    std::vector<float*> ptrs(1);
    std::vector<int> sizes(1);
    ptrs[0]  = train_samples;
    sizes[0] = params.rows;

    raft::spatial::knn::brute_force_knn(handle,
                                        ptrs,
                                        sizes,
                                        params.cols,
                                        train_samples,
                                        params.rows,
                                        knn_indices,
                                        knn_dists,
                                        params.k);

    std::vector<int*> y;
    y.push_back(train_labels);

    std::vector<int*> uniq_labels;
    uniq_labels.push_back(unique_labels.data());

    std::vector<int> n_unique;
    n_unique.push_back(n_classes);

    knn_classify(handle,
                 pred_labels,
                 knn_indices,
                 y,
                 params.rows,
                 params.rows,
                 params.k,
                 uniq_labels,
                 n_unique);

    CUDA_CHECK(cudaStreamSynchronize(stream));
  }

  void SetUp() override { basicTest(); }

  void TearDown() override
  {
    CUDA_CHECK(cudaFree(train_samples));
    CUDA_CHECK(cudaFree(train_labels));

    CUDA_CHECK(cudaFree(pred_labels));

    CUDA_CHECK(cudaFree(knn_indices));
    CUDA_CHECK(cudaFree(knn_dists));
  }

 protected:
  KNNClassifyInputs params;

  float* train_samples;
  int* train_labels;

  int* pred_labels;

  int64_t* knn_indices;
  float* knn_dists;
};

typedef KNNClassifyTest KNNClassifyTestF;
TEST_P(KNNClassifyTestF, Fit)
{
  ASSERT_TRUE(devArrMatch(train_labels, pred_labels, params.rows, raft::Compare<int>()));
}

const std::vector<KNNClassifyInputs> inputsf = {{100, 10, 2, 0.01f, 2},
                                                {1000, 10, 5, 0.01f, 2},
                                                {10000, 10, 5, 0.01f, 2},
                                                {100, 10, 2, 0.01f, 10},
                                                {1000, 10, 5, 0.01f, 10},
                                                {10000, 10, 5, 0.01f, 10},
                                                {100, 10, 2, 0.01f, 50},
                                                {1000, 10, 5, 0.01f, 50},
                                                {10000, 10, 5, 0.01f, 50}};

INSTANTIATE_TEST_CASE_P(KNNClassifyTest, KNNClassifyTestF, ::testing::ValuesIn(inputsf));

};  // end namespace Selection
};  // namespace MLCommon
