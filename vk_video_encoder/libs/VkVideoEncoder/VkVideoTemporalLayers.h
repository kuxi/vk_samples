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

#ifndef _VKVIDEOENCODER_VKVIDEOTEMPORALLAYERS_H_
#define _VKVIDEOENCODER_VKVIDEOTEMPORALLAYERS_H_

#include <assert.h>
#include <stdint.h>
#include <atomic>
#include <bitset>

class VkVideoTemporalLayers {

public:
    VkVideoTemporalLayers();

    void SetTemporalLayerCountToOne();
    void SetTemporalLayerCountToThree();

    uint32_t GetTemporalLayer(int temporal_idx) const;
    uint32_t GetTemporalPatternIdx() const;
    uint32_t GetTemporalPatternLength() const;
    uint8_t GetTemporalLayerCount() const;
    void BeforeEncode(bool is_keyframe);

    // Returns true if frames of the current temporal layer can reference frames of `other_temporal_layer`
    static bool CanReference(uint32_t current_temporal_index, uint32_t other_temporal_index);

    // Returns true if frames of the current temporal layer can be referenced by future frames
    bool CanBeReferenced(int temporal_idx) const;

private:
    uint8_t temporal_layer_count_;
    uint32_t pattern_index_;
    uint32_t pattern_length_;
};
#endif /* _VKVIDEOENCODER_VKVIDEOTEMPORALLAYERS_H_ */

