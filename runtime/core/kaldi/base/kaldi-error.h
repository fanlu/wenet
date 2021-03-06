// base/kaldi-error.h

// Copyright (c) 2021 Mobvoi Inc (Binbin Zhang)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef KALDI_BASE_KALDI_ERROR_H_
#define KALDI_BASE_KALDI_ERROR_H_ 1

#include "utils/log.h"

namespace kaldi {

#define KALDI_WARN \
  google::LogMessage(__FILE__, __LINE__, google::WARNING).stream()
#define KALDI_ERR \
  google::LogMessage(__FILE__, __LINE__, google::ERROR).stream()
#define KALDI_LOG \
  google::LogMessage(__FILE__, __LINE__, google::INFO).stream()
#define KALDI_VLOG(v) VLOG(v)

#define KALDI_ASSERT(condition) CHECK(condition)

}  // namespace kaldi

#endif  // KALDI_BASE_KALDI_ERROR_H_

