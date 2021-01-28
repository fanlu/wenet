// Copyright 2020 Mobvoi Inc. All Rights Reserved.
// Author: binbinzhang@mobvoi.com (Binbin Zhang)
#include <chrono>
#include <iomanip>
#include <string>

#include "decoder/symbol_table.h"
#include "decoder/torch_asr_decoder.h"
#include "decoder/torch_asr_model.h"
#include "frontend/feature_pipeline.h"
#include "frontend/wav.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "torch/script.h"
#include "torch/torch.h"

DEFINE_int32(num_bins, 80, "num mel bins for fbank feature");
DEFINE_int32(chunk_size, 16, "decoding chunk size");
DEFINE_string(model_path, "", "pytorch exported model path");
DEFINE_string(wav_scp, "", "input wav scp");
DEFINE_string(dict_path, "", "dict path");

int main(int argc, char *argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, false);
  google::InitGoogleLogging(argv[0]);

  auto model = std::make_shared<wenet::TorchAsrModel>();
  model->Read(FLAGS_model_path);
  wenet::SymbolTable symbol_table(FLAGS_dict_path);
  wenet::DecodeOptions decode_config;
  decode_config.chunk_size = FLAGS_chunk_size;
  wenet::FeaturePipelineConfig feature_config;
  feature_config.num_bins = FLAGS_num_bins;

  std::ifstream wav_scp(FLAGS_wav_scp);
  std::string line;
  unsigned int total_waves_dur = 0;
  unsigned int total_decode_time = 0;
  while (getline(wav_scp, line)) {
    std::string delimiter = " ";
    int pos = line.find(delimiter);
    std::string utt = line.substr(0, pos);
    std::string wav = line.substr(pos + delimiter.length(), line.length());

    wenet::WavReader wav_reader(wav);
    auto feature_pipeline =
        std::make_shared<wenet::FeaturePipeline>(feature_config);
    feature_pipeline->AcceptWaveform(std::vector<float>(
        wav_reader.data(), wav_reader.data() + wav_reader.num_sample()));
    feature_pipeline->set_input_finished();
    LOG(INFO) << "num frames " << feature_pipeline->num_frames();

    wenet::TorchAsrDecoder decoder(feature_pipeline, model, symbol_table,
                                   decode_config);

    unsigned int wave_dur = wav_reader.num_sample() / 16;
    unsigned int decode_time = 0;
    while (true) {
      auto start = std::chrono::steady_clock::now();
      bool finish = decoder.Decode();
      auto end = std::chrono::steady_clock::now();
      decode_time +=
          std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
              .count();
      if (finish) {
        break;
      } else {
        LOG(INFO) << "Partial result: " << decoder.result();
      }
    }
    LOG(INFO) << "Final result: " << decoder.result();
    std::cout << wave_dur << "ms\t" << decode_time << "ms\t"
              << std::setprecision(4)
              << static_cast<float>(decode_time) / wave_dur << "\t" << utt
              << "\t" << decoder.result() << std::endl;

    total_waves_dur += wave_dur;
    total_decode_time += decode_time;
  }
  std::cout << "Total: " << total_waves_dur << "ms " << total_decode_time
            << "ms." << std::endl;
  std::cout << "RTF: " << std::setprecision(4)
            << static_cast<float>(total_decode_time) / total_waves_dur
            << std::endl;
  return 0;
}
