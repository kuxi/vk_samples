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

#include "VkVideoTemporalLayers.h"

uint32_t PATTERN[4] = {
    0, 2, 1, 2
};

uint32_t PATTERN_LENGTH = 4;

VkVideoTemporalLayers::VkVideoTemporalLayers()
    : pattern_index_(0)
{ 
    SetTemporalLayerCountToOne();
}

void VkVideoTemporalLayers::SetTemporalLayerCountToOne() {
    temporal_layer_count_ = 1;
    pattern_length_ = 1;
}

void VkVideoTemporalLayers::SetTemporalLayerCountToThree() {
    temporal_layer_count_ = 3;
    pattern_length_ = PATTERN_LENGTH;
}

uint32_t VkVideoTemporalLayers::GetTemporalLayer(int temporal_idx) const {
    return PATTERN[temporal_idx];
}
uint32_t VkVideoTemporalLayers::GetTemporalPatternIdx() const {
    return pattern_index_;
}
uint32_t VkVideoTemporalLayers::GetTemporalPatternLength() const {
    return pattern_length_;
}
uint8_t VkVideoTemporalLayers::GetTemporalLayerCount() const {
    return temporal_layer_count_;
}

void VkVideoTemporalLayers::BeforeEncode(bool is_keyframe) {
    if (is_keyframe) {
        pattern_index_ = 0;
    } else {
        pattern_index_ = (pattern_index_ + 1) % pattern_length_;
    }
}

bool VkVideoTemporalLayers::CanReference(uint32_t current_temporal_index, uint32_t other_temporal_index) {
    // we want the following pattern
    //     2     2
    //    /     /
    //   /   1-/
    //  /   /   
    // 0-----------0 ....
    if (current_temporal_index == 3) {
        return other_temporal_index == 2;
    } else if (other_temporal_index == 0) {
        return true;
    } else {
        return false;
    }
}

bool VkVideoTemporalLayers::CanBeReferenced(int temporal_idx) const {
    return PATTERN[temporal_idx] < 2;
}
