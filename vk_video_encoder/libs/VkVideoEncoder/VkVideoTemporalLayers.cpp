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

int PATTERN[4] = {
    0, 2, 1, 2
};

int PATTERN_LENGTH = 4;

VkVideoTemporalLayers::VkVideoTemporalLayers()
    : temporal_layer_count_(1)
    , pattern_index_(0)
{ }

void VkVideoTemporalLayers::SetTemporalLayerCountToThree() {
    temporal_layer_count_ = 3;
}

int VkVideoTemporalLayers::GetTemporalLayer() const {
    return PATTERN[pattern_index_];
}
int VkVideoTemporalLayers::GetTemporalPatternIdx() const {
    return pattern_index_;
}

void VkVideoTemporalLayers::BeforeEncode(bool is_keyframe) {
    if (is_keyframe) {
        pattern_index_ = 0;
    } else {
        pattern_index_ = (pattern_index_ + 1) % PATTERN_LENGTH;
    }
}

bool VkVideoTemporalLayers::CanReference(int current_pattern_idx, int other_pattern_idx) {
    // we want the following pattern
    //     2     2
    //    /     /
    //   /   1-/
    //  /   /   
    // 0-----------0 ....
    if (current_pattern_idx < 0 || current_pattern_idx >= PATTERN_LENGTH || other_pattern_idx < 0 || other_pattern_idx >= PATTERN_LENGTH) {
        assert(!"Invalid pattern index");
        return false;
    }
    if (current_pattern_idx == 3) {
        return other_pattern_idx == 2;
    } else if (other_pattern_idx == 0) {
        return true;
    } else {
        return false;
    }
}

