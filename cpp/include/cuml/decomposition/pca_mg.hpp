/*
 * Copyright (c) 2020-2021, NVIDIA CORPORATION.
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

#pragma once

#include <opg/matrix/data.hpp>
#include <opg/matrix/part_descriptor.hpp>
#include "pca.hpp"
namespace ML {

enum class mg_solver { COV_EIG_DQ, COV_EIG_JACOBI, QR };

typedef paramsTSVDTemplate<mg_solver> paramsTSVDMG;

typedef paramsPCATemplate<mg_solver> paramsPCAMG;

namespace PCA {
namespace opg {

/**
 * @brief performs MNMG fit operation for the pca
 * @param[in] handle: the internal cuml handle object
 * @param[in] input_data: input data
 * @param[in] input_desc: descriptor for input data
 * @param[out] components: principal components of the input data
 * @param[out] explained_var: explained var
 * @param[out] explained_var_ratio: the explained var ratio
 * @param[out] singular_vals: singular values of the data
 * @param[out] mu: mean of every column in input
 * @param[out] noise_vars: variance of the noise
 * @param[in] prms: data structure that includes all the parameters from input size to algorithm
 * @param[in] verbose
 */
void fit(raft::handle_t& handle,
         std::vector<MLCommon::Matrix::Data<float>*>& input_data,
         MLCommon::Matrix::PartDescriptor& input_desc,
         float* components,
         float* explained_var,
         float* explained_var_ratio,
         float* singular_vals,
         float* mu,
         float* noise_vars,
         paramsPCAMG prms,
         bool verbose = false);

void fit(raft::handle_t& handle,
         std::vector<MLCommon::Matrix::Data<double>*>& input_data,
         MLCommon::Matrix::PartDescriptor& input_desc,
         double* components,
         double* explained_var,
         double* explained_var_ratio,
         double* singular_vals,
         double* mu,
         double* noise_vars,
         paramsPCAMG prms,
         bool verbose = false);

/**
 * @brief performs MNMG fit and transform operation for the pca
 * @param[in] handle: the internal cuml handle object
 * @param[in] rank_sizes: includes all the partition size information for the rank
 * @param[in] n_parts: number of partitions
 * @param[in] input: input data
 * @param[out] trans_input: transformed input data
 * @param[out] components: principal components of the input data
 * @param[out] explained_var: explained var
 * @param[out] explained_var_ratio: the explained var ratio
 * @param[out] singular_vals: singular values of the data
 * @param[out] mu: mean of every column in input
 * @param[out] noise_vars: variance of the noise
 * @param[in] prms: data structure that includes all the parameters from input size to algorithm
 * @param[in] verbose
 */
void fit_transform(raft::handle_t& handle,
                   MLCommon::Matrix::RankSizePair** rank_sizes,
                   std::uint32_t n_parts,
                   MLCommon::Matrix::floatData_t** input,
                   MLCommon::Matrix::floatData_t** trans_input,
                   float* components,
                   float* explained_var,
                   float* explained_var_ratio,
                   float* singular_vals,
                   float* mu,
                   float* noise_vars,
                   paramsPCAMG prms,
                   bool verbose);

void fit_transform(raft::handle_t& handle,
                   MLCommon::Matrix::RankSizePair** rank_sizes,
                   std::uint32_t n_parts,
                   MLCommon::Matrix::doubleData_t** input,
                   MLCommon::Matrix::doubleData_t** trans_input,
                   double* components,
                   double* explained_var,
                   double* explained_var_ratio,
                   double* singular_vals,
                   double* mu,
                   double* noise_vars,
                   paramsPCAMG prms,
                   bool verbose);

/**
 * @brief performs MNMG transform operation for the pca
 * @param[in] handle: the internal cuml handle object
 * @param[in] rank_sizes: includes all the partition size information for the rank
 * @param[in] n_parts: number of partitions
 * @param[in] input: input data
 * @param[in] components: principal components of the input data
 * @param[out] trans_input: transformed input data
 * @param[in] singular_vals: singular values of the data
 * @param[in] mu: mean of every column in input
 * @param[in] prms: data structure that includes all the parameters from input size to algorithm
 * @param[in] verbose
 */
void transform(raft::handle_t& handle,
               MLCommon::Matrix::RankSizePair** rank_sizes,
               std::uint32_t n_parts,
               MLCommon::Matrix::Data<float>** input,
               float* components,
               MLCommon::Matrix::Data<float>** trans_input,
               float* singular_vals,
               float* mu,
               paramsPCAMG prms,
               bool verbose);

void transform(raft::handle_t& handle,
               MLCommon::Matrix::RankSizePair** rank_sizes,
               std::uint32_t n_parts,
               MLCommon::Matrix::Data<double>** input,
               double* components,
               MLCommon::Matrix::Data<double>** trans_input,
               double* singular_vals,
               double* mu,
               paramsPCAMG prms,
               bool verbose);

/**
 * @brief performs MNMG inverse transform operation for the pca
 * @param[in] handle: the internal cuml handle object
 * @param[in] rank_sizes: includes all the partition size information for the rank
 * @param[in] n_parts: number of partitions
 * @param[in] trans_input: transformed input data
 * @param[in] components: principal components of the input data
 * @param[out] input: input data
 * @param[in] singular_vals: singular values of the data
 * @param[in] mu: mean of every column in input
 * @param[in] prms: data structure that includes all the parameters from input size to algorithm
 * @param[in] verbose
 */
void inverse_transform(raft::handle_t& handle,
                       MLCommon::Matrix::RankSizePair** rank_sizes,
                       std::uint32_t n_parts,
                       MLCommon::Matrix::Data<float>** trans_input,
                       float* components,
                       MLCommon::Matrix::Data<float>** input,
                       float* singular_vals,
                       float* mu,
                       paramsPCAMG prms,
                       bool verbose);

void inverse_transform(raft::handle_t& handle,
                       MLCommon::Matrix::RankSizePair** rank_sizes,
                       std::uint32_t n_parts,
                       MLCommon::Matrix::Data<double>** trans_input,
                       double* components,
                       MLCommon::Matrix::Data<double>** input,
                       double* singular_vals,
                       double* mu,
                       paramsPCAMG prms,
                       bool verbose);

};  // end namespace opg
};  // end namespace PCA
};  // end namespace ML
