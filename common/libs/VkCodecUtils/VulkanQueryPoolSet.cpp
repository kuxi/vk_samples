/*
* Copyright 2024 NVIDIA Corporation.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <iostream>
#include "VkCodecUtils/VulkanQueryPoolSet.h"

namespace {

#define LOGGER std::cout

void PrintUnexpectedPNext(const void* pNext, int indent) {
  std::string indent_str(indent, ' ');
  LOGGER << indent_str << "  #### Unexpected pNext value" << std::endl;
  VkBaseInStructure base_in = *reinterpret_cast<const VkBaseInStructure*>(pNext);
  LOGGER << indent_str << "  base_in.sType: " << base_in.sType << std::endl;
  LOGGER << indent_str << "  base_in.pNext: " << base_in.pNext << std::endl;
}

void PrintTuningMode(const VkVideoEncodeTuningModeKHR& tuning_mode, int indent) {
  std::string indent_str(indent, ' ');
  switch (tuning_mode) {
    case VK_VIDEO_ENCODE_TUNING_MODE_DEFAULT_KHR:
      LOGGER << indent_str << "eDefault" << std::endl;
      break;
    case VK_VIDEO_ENCODE_TUNING_MODE_HIGH_QUALITY_KHR:
      LOGGER << indent_str << "eHighQuality" << std::endl;
      break;
    case VK_VIDEO_ENCODE_TUNING_MODE_LOSSLESS_KHR:
      LOGGER << indent_str << "eLossless" << std::endl;
      break;
    case VK_VIDEO_ENCODE_TUNING_MODE_LOW_LATENCY_KHR:
      LOGGER << indent_str << "eLowLatency" << std::endl;
      break;
    case VK_VIDEO_ENCODE_TUNING_MODE_ULTRA_LOW_LATENCY_KHR:
      LOGGER << indent_str << "eUltraLowLatency" << std::endl;
      break;
    case VK_VIDEO_ENCODE_TUNING_MODE_MAX_ENUM_KHR:
      LOGGER << indent_str << "eMaxEnum" << std::endl;
      break;
  }
}

void PrintVideoEncodeUsageFlags(const VkVideoEncodeUsageFlagsKHR& flags, int indent) {
  std::string indent_str(indent, ' ');
  LOGGER << indent_str << "eDefault: " << (flags & VK_VIDEO_ENCODE_USAGE_DEFAULT_KHR) << std::endl;
  LOGGER << indent_str << "eConferencing: " << (flags & VK_VIDEO_ENCODE_USAGE_CONFERENCING_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eRecording: " << (flags & VK_VIDEO_ENCODE_USAGE_RECORDING_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eStreaming: " << (flags & VK_VIDEO_ENCODE_USAGE_STREAMING_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eTranscoding: " << (flags & VK_VIDEO_ENCODE_USAGE_TRANSCODING_BIT_KHR) << std::endl;
}

void PrintVideoContentHints(const VkVideoEncodeContentFlagsKHR& content_hints, int indent) {
  std::string indent_str(indent, ' ');
  LOGGER << indent_str << "eDefault: " << (content_hints & VK_VIDEO_ENCODE_CONTENT_DEFAULT_KHR) << std::endl;
  LOGGER << indent_str << "eCamera: " << (content_hints & VK_VIDEO_ENCODE_CONTENT_CAMERA_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eDesktop: " << (content_hints & VK_VIDEO_ENCODE_CONTENT_DESKTOP_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eRendered: " << (content_hints & VK_VIDEO_ENCODE_CONTENT_RENDERED_BIT_KHR) << std::endl;
}

void PrintEncodeUsageInfo(const VkVideoEncodeUsageInfoKHR& encode_usage_info, int indent) {
  std::string indent_str(indent, ' ');
  LOGGER << indent_str << "encode_usage_info.videoUsageHints: " << std::endl;
  PrintVideoEncodeUsageFlags(encode_usage_info.videoUsageHints, indent + 2);
  LOGGER << indent_str << "encode_usage_info.videoContentHints: " << std::endl;
  PrintVideoContentHints(encode_usage_info.videoContentHints, indent + 2);
  LOGGER << indent_str << "encode_usage_info.tuningMode: " << std::endl;
  PrintTuningMode(encode_usage_info.tuningMode, indent + 2);
  LOGGER << indent_str << "encode_usage_info.pNext: " << encode_usage_info.pNext << std::endl;
  if (encode_usage_info.pNext != nullptr) {
    PrintUnexpectedPNext(encode_usage_info.pNext, indent + 2);
  }
}

void PrintAv1ProfileInfo(const VkVideoEncodeAV1ProfileInfoKHR& av1_profile_info, int indent) {
  std::string indent_str(indent, ' ');
  LOGGER << indent_str << "av1_profile_info.stdProfile: " << av1_profile_info.stdProfile << std::endl;
  LOGGER << indent_str << "av1_profile_info.pNext: " << av1_profile_info.pNext << std::endl;
  if (av1_profile_info.pNext != nullptr) {
    const VkVideoEncodeUsageInfoKHR* encode_usage_info =
        reinterpret_cast<const VkVideoEncodeUsageInfoKHR*>(av1_profile_info.pNext);
    if (encode_usage_info != nullptr) {
      PrintEncodeUsageInfo(*encode_usage_info, indent + 2);
    } else {
      LOGGER << indent_str << "  av1_profile_info.pNext is not a VideoEncodeUsageInfoKHR" << std::endl;
    }
  }
}

void PrintComponentBitDepthFlags(const VkVideoComponentBitDepthFlagsKHR& flags, int indent) {
  std::string indent_str(indent, ' ');
  LOGGER << indent_str << "e8: " << (flags & VK_VIDEO_COMPONENT_BIT_DEPTH_8_BIT_KHR) << std::endl;
  LOGGER << indent_str << "e10: " << (flags & VK_VIDEO_COMPONENT_BIT_DEPTH_10_BIT_KHR) << std::endl;
  LOGGER << indent_str << "e12: " << (flags & VK_VIDEO_COMPONENT_BIT_DEPTH_12_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eInvalid: " << (flags & VK_VIDEO_COMPONENT_BIT_DEPTH_INVALID_KHR) << std::endl;
}

void PrintSubsamplingFlags(const VkVideoChromaSubsamplingFlagsKHR& flags, int indent) {
  std::string indent_str(indent, ' ');
  LOGGER << indent_str << "eMonochrome: " << (flags & VK_VIDEO_CHROMA_SUBSAMPLING_MONOCHROME_BIT_KHR) << std::endl;
  LOGGER << indent_str << "e420: " << (flags & VK_VIDEO_CHROMA_SUBSAMPLING_420_BIT_KHR) << std::endl;
  LOGGER << indent_str << "e422: " << (flags & VK_VIDEO_CHROMA_SUBSAMPLING_422_BIT_KHR) << std::endl;
  LOGGER << indent_str << "e444: " << (flags & VK_VIDEO_CHROMA_SUBSAMPLING_444_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eInvalid: " << (flags & VK_VIDEO_CHROMA_SUBSAMPLING_INVALID_KHR) << std::endl;
}

void PrintVideoCodecOperationFlags(const VkVideoCodecOperationFlagsKHR& flags, int indent) {
  std::string indent_str(indent, ' ');
  LOGGER << indent_str << "eEncodeAv1: " << (flags & VK_VIDEO_CODEC_OPERATION_ENCODE_AV1_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eDecodeAv1: " << (flags & VK_VIDEO_CODEC_OPERATION_DECODE_AV1_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eEncodeH264: " << (flags & VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eDecodeH264: " << (flags & VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eEncodeH265: " << (flags & VK_VIDEO_CODEC_OPERATION_ENCODE_H265_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eDecodeH265: " << (flags & VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eNone: " << (flags & VK_VIDEO_CODEC_OPERATION_NONE_KHR) << std::endl;
}

void PrintVideoProfileInfo(const VkVideoProfileInfoKHR& video_profile_info, int indent) {
  std::string indent_str(indent, ' ');
  LOGGER << indent_str << "video_profile_info.videoCodecOperation: " << std::endl;
  PrintVideoCodecOperationFlags(video_profile_info.videoCodecOperation, indent + 2);
  LOGGER << indent_str << "video_profile_info.lumaBitDepth: " << std::endl;
  PrintComponentBitDepthFlags(video_profile_info.lumaBitDepth, indent + 2);
  LOGGER << indent_str << "video_profile_info.chromaBitDepth: " << std::endl;
  PrintComponentBitDepthFlags(video_profile_info.chromaBitDepth, indent + 2);
  LOGGER << indent_str << "video_profile_info.chromaSubsampling: " << std::endl;
  PrintSubsamplingFlags(video_profile_info.chromaSubsampling, indent + 2);
  LOGGER << indent_str << "video_profile_info.pNext: " << video_profile_info.pNext << std::endl;
  if (video_profile_info.pNext != nullptr) {
    const VkVideoEncodeAV1ProfileInfoKHR* av1_profile_info =
        reinterpret_cast<const VkVideoEncodeAV1ProfileInfoKHR*>(video_profile_info.pNext);
    if (av1_profile_info != nullptr) {
      PrintAv1ProfileInfo(*av1_profile_info, indent + 2);
    } else {
      LOGGER << "video_profile_info.pNext was not an AV1 profile" << std::endl;
    }
  }
}

void PrintEncodeFeedbackFlags(const VkVideoEncodeFeedbackFlagsKHR& flags, int indent) {
  std::string indent_str(indent, ' ');
  LOGGER << indent_str << "eBitstreamBufferOffset: " << (flags & VK_VIDEO_ENCODE_FEEDBACK_BITSTREAM_BUFFER_OFFSET_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eBitstreamBytesWritten: " << (flags & VK_VIDEO_ENCODE_FEEDBACK_BITSTREAM_BYTES_WRITTEN_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eBitstreamHasOverrides: " << (flags & VK_VIDEO_ENCODE_FEEDBACK_BITSTREAM_HAS_OVERRIDES_BIT_KHR) << std::endl;
}

void PrintQueryPoolFeedbackCreateInfo(const VkQueryPoolVideoEncodeFeedbackCreateInfoKHR& create_info, int indent) {
  std::string indent_str(indent, ' ');
  LOGGER << indent_str << "create_info.encodeFeedbackFlags: " << std::endl;
  PrintEncodeFeedbackFlags(create_info.encodeFeedbackFlags, indent + 2);
  LOGGER << indent_str << "create_info.pNext: " << create_info.pNext << std::endl;
  if (create_info.pNext != nullptr) {
    const VkVideoProfileInfoKHR* video_profile_info =
        reinterpret_cast<const VkVideoProfileInfoKHR*>(create_info.pNext);
    if (video_profile_info != nullptr) {
      PrintVideoProfileInfo(*video_profile_info, indent + 2);
    } else {
      LOGGER << "  create_info.pNext is not a VideoProfileInfoKHR";
    }
  }
}

void PrintQueryPoolCreateInfo(const VkQueryPoolCreateInfo& create_info) {
  LOGGER << "query_pool_create_info.flags: " << std::endl;
  LOGGER << "query_pool_create_info.queryType: " << create_info.queryType << std::endl;
  LOGGER << "query_pool_create_info.queryCount: " << create_info.queryCount << std::endl;
  LOGGER << "query_pool_create_info.pipelineStatistics: " << create_info.pipelineStatistics << std::endl;
  LOGGER << "query_pool_create_info.pNext: " << create_info.pNext << std::endl;
  if (create_info.pNext != nullptr) {
    const VkQueryPoolVideoEncodeFeedbackCreateInfoKHR* feedback_info =
        reinterpret_cast<const VkQueryPoolVideoEncodeFeedbackCreateInfoKHR*>(create_info.pNext);
    if (feedback_info != nullptr) {
      LOGGER << "  query_pool_create_info.pNext is a QueryPoolVideoEncodeFeedbackCreateInfoKHR" << std::endl;
      PrintQueryPoolFeedbackCreateInfo(*feedback_info, 2);
    } else {
      LOGGER << "  query_pool_create_info.pNext is not a QueryPoolVideoEncodeFeedbackCreateInfoKHR" << std::endl;
    }
  }
}
} // namespace


VkResult VulkanQueryPoolSet::CreateSet(const VulkanDeviceContext* vkDevCtx, uint32_t queryCount,
                       VkQueryType queryType,
                       VkQueryPoolCreateFlags flags,
                       const void* pNext) {
        DestroySet();

        VkQueryPoolCreateInfo queryPoolCreateInfo = {VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO};
        queryPoolCreateInfo.queryType = queryType;
        queryPoolCreateInfo.queryCount = queryCount;
        queryPoolCreateInfo.pNext = pNext;

        PrintQueryPoolCreateInfo(queryPoolCreateInfo);
        VkResult result = vkDevCtx->CreateQueryPool(*vkDevCtx, &queryPoolCreateInfo, NULL, &m_queryPool);
        if (result != VK_SUCCESS) {
            assert(!"Failed to create query pool!");
            return result;
        }

        m_queryCount = queryCount;
        m_vkDevCtx   = vkDevCtx;

        return VK_SUCCESS;
}
