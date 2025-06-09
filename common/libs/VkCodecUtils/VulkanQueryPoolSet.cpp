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
    PrintUnexpectedPNext(create_info.pNext, indent + 2);
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
