/*
* Copyright 2021 NVIDIA Corporation.
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

#include <assert.h>
#include <string.h>
#include "vk_video/vulkan_video_codecs_common.h"
#include "VkCodecUtils/Helpers.h"
#include "VkCodecUtils/HelpersDispatchTable.h"
#include "VkCodecUtils/VulkanDeviceContext.h"
#include "VkVideoCore/VkVideoCoreProfile.h"
#include "VkCodecUtils/VulkanVideoSession.h"

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

void PrintVideoSessionCreateFlags(const VkVideoSessionCreateFlagsKHR& flags, int indent) {
  std::string indent_str(indent, ' ');
  // TODO: Missing?
  //LOGGER << indent_str << "eInlineSessionParameters: " << (flags & VK_VIDEO_SESSION_CREATE_INLINE_SESSION_PARAMETERS_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eInlineQueries: " << (flags & VK_VIDEO_SESSION_CREATE_INLINE_QUERIES_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eAllowEncodeEmphasisMap: " << (flags & VK_VIDEO_SESSION_CREATE_ALLOW_ENCODE_EMPHASIS_MAP_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eAllowEncodeQuantizationDeltaMap: " << (flags & VK_VIDEO_SESSION_CREATE_ALLOW_ENCODE_QUANTIZATION_DELTA_MAP_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eAllowEncodeParameterOptimizations: " << (flags & VK_VIDEO_SESSION_CREATE_ALLOW_ENCODE_PARAMETER_OPTIMIZATIONS_BIT_KHR) << std::endl;
  LOGGER << indent_str << "eProtectedContent: " << (flags & VK_VIDEO_SESSION_CREATE_PROTECTED_CONTENT_BIT_KHR) << std::endl;
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

void PrintExtensionProperties(const VkExtensionProperties& pStdHeaderVersion, int indent) {
  std::string indent_str(indent, ' ');
  LOGGER << indent_str << "pStdHeaderVersion.extensionName: " << pStdHeaderVersion.extensionName << std::endl;
  LOGGER << indent_str << "pStdHeaderVersion.specVersion: " << pStdHeaderVersion.specVersion << std::endl;
}

void PrintVideoSessionCreateInfo(const VkVideoSessionCreateInfoKHR& create_info) {
  LOGGER << "create_info.queueFamilyIndex: " << create_info.queueFamilyIndex << std::endl;
  LOGGER << "create_info.flags: " << std::endl;
  PrintVideoSessionCreateFlags(create_info.flags, 2);
  LOGGER << "create_info.pVideoProfile: " << create_info.pVideoProfile << std::endl;
  if (create_info.pVideoProfile != nullptr) {
    PrintVideoProfileInfo(*create_info.pVideoProfile, 2);
  } else {
    LOGGER << "  create_info.pVideoProfile is null" << std::endl;
  }
  LOGGER << "create_info.pictureFormat: " << create_info.pictureFormat << std::endl;
  LOGGER << "create_info.maxCodedExtent: " << create_info.maxCodedExtent.width << " " << create_info.maxCodedExtent.height << std::endl;
  LOGGER << "create_info.referencePictureFormat: " << create_info.referencePictureFormat << std::endl;
  LOGGER << "create_info.maxDpbSlots: " << create_info.maxDpbSlots << std::endl;
  LOGGER << "create_info.maxActiveReferencePictures: " << create_info.maxActiveReferencePictures << std::endl;
  LOGGER << "create_info.pStdHeaderVersion: " << create_info.pStdHeaderVersion << std::endl;
  if (create_info.pStdHeaderVersion != nullptr) {
    PrintExtensionProperties(*create_info.pStdHeaderVersion, 2);
  }
  LOGGER << "create_info.pNext: " << create_info.pNext << std::endl;
  if (create_info.pNext != nullptr) {
    PrintUnexpectedPNext(create_info.pNext, 2);
  }
}
} // namespace

VkResult VulkanVideoSession::Create(const VulkanDeviceContext* vkDevCtx,
                                    VkVideoSessionCreateFlagsKHR sessionCreateFlags,
                                    uint32_t            videoQueueFamily,
                                    VkVideoCoreProfile* pVideoProfile,
                                    VkFormat            pictureFormat,
                                    const VkExtent2D&   maxCodedExtent,
                                    VkFormat            referencePicturesFormat,
                                    uint32_t            maxDpbSlots,
                                    uint32_t            maxActiveReferencePictures,
                                    VkSharedBaseObj<VulkanVideoSession>& videoSession)
{
    VulkanVideoSession* pNewVideoSession = new VulkanVideoSession(vkDevCtx, pVideoProfile);

    static const VkExtensionProperties h264DecodeStdExtensionVersion = { VK_STD_VULKAN_VIDEO_CODEC_H264_DECODE_EXTENSION_NAME, VK_STD_VULKAN_VIDEO_CODEC_H264_DECODE_SPEC_VERSION };
    static const VkExtensionProperties h265DecodeStdExtensionVersion = { VK_STD_VULKAN_VIDEO_CODEC_H265_DECODE_EXTENSION_NAME, VK_STD_VULKAN_VIDEO_CODEC_H265_DECODE_SPEC_VERSION };
    static const VkExtensionProperties av1DecodeStdExtensionVersion =  { VK_STD_VULKAN_VIDEO_CODEC_AV1_DECODE_EXTENSION_NAME, VK_STD_VULKAN_VIDEO_CODEC_AV1_DECODE_SPEC_VERSION };
    static const VkExtensionProperties h264EncodeStdExtensionVersion = { VK_STD_VULKAN_VIDEO_CODEC_H264_ENCODE_EXTENSION_NAME, VK_STD_VULKAN_VIDEO_CODEC_H264_ENCODE_SPEC_VERSION };
    static const VkExtensionProperties h265EncodeStdExtensionVersion = { VK_STD_VULKAN_VIDEO_CODEC_H265_ENCODE_EXTENSION_NAME, VK_STD_VULKAN_VIDEO_CODEC_H265_ENCODE_SPEC_VERSION };
    static const VkExtensionProperties av1EncodeStdExtensionVersion =  { VK_STD_VULKAN_VIDEO_CODEC_AV1_ENCODE_EXTENSION_NAME, VK_STD_VULKAN_VIDEO_CODEC_AV1_ENCODE_SPEC_VERSION };

    VkVideoSessionCreateInfoKHR& createInfo = pNewVideoSession->m_createInfo;
    createInfo.flags = sessionCreateFlags;
    createInfo.pVideoProfile = pVideoProfile->GetProfile();
    createInfo.queueFamilyIndex = videoQueueFamily;
    createInfo.pictureFormat = pictureFormat;
    createInfo.maxCodedExtent = maxCodedExtent;
    createInfo.maxDpbSlots = maxDpbSlots + 1;
    createInfo.maxActiveReferencePictures = maxActiveReferencePictures;
    createInfo.referencePictureFormat = referencePicturesFormat;

    switch ((int32_t)pVideoProfile->GetCodecType()) {
    case VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR:
        createInfo.pStdHeaderVersion = &h264DecodeStdExtensionVersion;
        break;
    case VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR:
        createInfo.pStdHeaderVersion = &h265DecodeStdExtensionVersion;
        break;
    case VK_VIDEO_CODEC_OPERATION_DECODE_AV1_BIT_KHR:
        createInfo.pStdHeaderVersion = &av1DecodeStdExtensionVersion;
        break;
    case VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_KHR:
        createInfo.pStdHeaderVersion = &h264EncodeStdExtensionVersion;
        break;
    case VK_VIDEO_CODEC_OPERATION_ENCODE_H265_BIT_KHR:
        createInfo.pStdHeaderVersion = &h265EncodeStdExtensionVersion;
        break;
    case VK_VIDEO_CODEC_OPERATION_ENCODE_AV1_BIT_KHR:
        createInfo.pStdHeaderVersion = &av1EncodeStdExtensionVersion;
        break;
    default:
        assert(0);
    }
    PrintVideoSessionCreateInfo(createInfo);
    VkResult result = vkDevCtx->CreateVideoSessionKHR(*vkDevCtx, &createInfo, NULL, &pNewVideoSession->m_videoSession);
    if (result != VK_SUCCESS) {
        return result;
    }

    uint32_t videoSessionMemoryRequirementsCount = 0;
    VkVideoSessionMemoryRequirementsKHR decodeSessionMemoryRequirements[MAX_BOUND_MEMORY];
    // Get the count first
    result = vkDevCtx->GetVideoSessionMemoryRequirementsKHR(*vkDevCtx, pNewVideoSession->m_videoSession,
        &videoSessionMemoryRequirementsCount, NULL);
    assert(result == VK_SUCCESS);
    assert(videoSessionMemoryRequirementsCount <= MAX_BOUND_MEMORY);

    memset(decodeSessionMemoryRequirements, 0x00, sizeof(decodeSessionMemoryRequirements));
    for (uint32_t i = 0; i < videoSessionMemoryRequirementsCount; i++) {
        decodeSessionMemoryRequirements[i].sType = VK_STRUCTURE_TYPE_VIDEO_SESSION_MEMORY_REQUIREMENTS_KHR;
    }

    result = vkDevCtx->GetVideoSessionMemoryRequirementsKHR(*vkDevCtx, pNewVideoSession->m_videoSession,
                                                            &videoSessionMemoryRequirementsCount,
                                                            decodeSessionMemoryRequirements);
    if (result != VK_SUCCESS) {
        return result;
    }

    uint32_t decodeSessionBindMemoryCount = videoSessionMemoryRequirementsCount;
    VkBindVideoSessionMemoryInfoKHR decodeSessionBindMemory[MAX_BOUND_MEMORY];

    for (uint32_t memIdx = 0; memIdx < decodeSessionBindMemoryCount; memIdx++) {

        uint32_t memoryTypeIndex = 0;
        uint32_t memoryTypeBits = decodeSessionMemoryRequirements[memIdx].memoryRequirements.memoryTypeBits;
        if (memoryTypeBits == 0) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        // Find an available memory type that satisfies the requested properties.
        for (; !(memoryTypeBits & 1); memoryTypeIndex++  ) {
            memoryTypeBits >>= 1;
        }

        VkMemoryAllocateInfo memInfo = {
            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,                          // sType
            NULL,                                                            // pNext
            decodeSessionMemoryRequirements[memIdx].memoryRequirements.size, // allocationSize
            memoryTypeIndex,                                                 // memoryTypeIndex
        };

        result = vkDevCtx->AllocateMemory(*vkDevCtx, &memInfo, 0,
                                                   &pNewVideoSession->m_memoryBound[memIdx]);
        if (result != VK_SUCCESS) {
            return result;
        }

        assert(result == VK_SUCCESS);
        decodeSessionBindMemory[memIdx].pNext = NULL;
        decodeSessionBindMemory[memIdx].sType = VK_STRUCTURE_TYPE_BIND_VIDEO_SESSION_MEMORY_INFO_KHR;
        decodeSessionBindMemory[memIdx].memory = pNewVideoSession->m_memoryBound[memIdx];

        decodeSessionBindMemory[memIdx].memoryBindIndex = decodeSessionMemoryRequirements[memIdx].memoryBindIndex;
        decodeSessionBindMemory[memIdx].memoryOffset = 0;
        decodeSessionBindMemory[memIdx].memorySize = decodeSessionMemoryRequirements[memIdx].memoryRequirements.size;
    }

    result = vkDevCtx->BindVideoSessionMemoryKHR(*vkDevCtx, pNewVideoSession->m_videoSession, decodeSessionBindMemoryCount,
                                                 decodeSessionBindMemory);
    assert(result == VK_SUCCESS);

    videoSession = pNewVideoSession;

    // Make sure we do not use dangling (on the stack) pointers
    createInfo.pNext = nullptr;

    return result;
}
