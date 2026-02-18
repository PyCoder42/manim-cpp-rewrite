#include "manim_cpp/animation/composition.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace manim_cpp::animation {

ParallelAnimation::ParallelAnimation(std::vector<Animation*> animations)
    : animations_(std::move(animations)) {
  double max_duration = 1.0;
  for (Animation* animation : animations_) {
    if (animation == nullptr) {
      continue;
    }
    max_duration = std::max(max_duration, animation->run_time_seconds());
  }
  set_run_time_seconds(max_duration);
}

void ParallelAnimation::begin() {
  for (Animation* animation : animations_) {
    if (animation == nullptr) {
      continue;
    }
    animation->begin();
  }
}

void ParallelAnimation::interpolate(const double alpha) {
  for (Animation* animation : animations_) {
    if (animation == nullptr) {
      continue;
    }
    animation->interpolate_with_rate(alpha);
  }
}

void ParallelAnimation::finish() {
  for (Animation* animation : animations_) {
    if (animation == nullptr) {
      continue;
    }
    animation->finish();
  }
}

SuccessionAnimation::SuccessionAnimation(std::vector<Animation*> animations)
    : animations_(std::move(animations)) {
  start_times_.reserve(animations_.size());
  durations_.reserve(animations_.size());

  total_duration_ = 0.0;
  for (Animation* animation : animations_) {
    start_times_.push_back(total_duration_);
    if (animation == nullptr) {
      durations_.push_back(0.0);
      continue;
    }
    const double duration = std::max(0.0, animation->run_time_seconds());
    durations_.push_back(duration);
    total_duration_ += duration;
  }
  if (total_duration_ <= 0.0) {
    total_duration_ = 1.0;
  }
  set_run_time_seconds(total_duration_);
}

void SuccessionAnimation::begin() {
  for (Animation* animation : animations_) {
    if (animation == nullptr) {
      continue;
    }
    animation->begin();
  }
}

void SuccessionAnimation::interpolate(const double alpha) {
  const double absolute_time =
      std::clamp(alpha, 0.0, 1.0) * total_duration_;

  for (std::size_t i = 0; i < animations_.size(); ++i) {
    Animation* animation = animations_[i];
    if (animation == nullptr) {
      continue;
    }

    const double start = start_times_[i];
    const double duration = durations_[i];
    double local_alpha = 1.0;
    if (duration > 0.0) {
      local_alpha = (absolute_time - start) / duration;
    }
    local_alpha = std::clamp(local_alpha, 0.0, 1.0);
    animation->interpolate_with_rate(local_alpha);
  }
}

void SuccessionAnimation::finish() {
  for (Animation* animation : animations_) {
    if (animation == nullptr) {
      continue;
    }
    animation->finish();
  }
}

LaggedStartAnimation::LaggedStartAnimation(std::vector<Animation*> animations,
                                           const double lag_ratio)
    : animations_(std::move(animations)), lag_ratio_(lag_ratio) {
  if (lag_ratio_ < 0.0) {
    throw std::invalid_argument("lag_ratio must be non-negative");
  }

  start_times_.reserve(animations_.size());
  durations_.reserve(animations_.size());

  total_duration_ = 0.0;
  for (std::size_t i = 0; i < animations_.size(); ++i) {
    const Animation* animation = animations_[i];
    const double duration =
        animation == nullptr ? 0.0 : std::max(0.0, animation->run_time_seconds());
    durations_.push_back(duration);

    if (i == 0) {
      start_times_.push_back(0.0);
    } else {
      const double previous_start = start_times_[i - 1];
      const double previous_duration = durations_[i - 1];
      start_times_.push_back(previous_start + (lag_ratio_ * previous_duration));
    }

    total_duration_ = std::max(total_duration_, start_times_.back() + duration);
  }
  if (total_duration_ <= 0.0) {
    total_duration_ = 1.0;
  }
  set_run_time_seconds(total_duration_);
}

void LaggedStartAnimation::begin() {
  for (Animation* animation : animations_) {
    if (animation == nullptr) {
      continue;
    }
    animation->begin();
  }
}

void LaggedStartAnimation::interpolate(const double alpha) {
  const double absolute_time = std::clamp(alpha, 0.0, 1.0) * total_duration_;
  for (std::size_t i = 0; i < animations_.size(); ++i) {
    Animation* animation = animations_[i];
    if (animation == nullptr) {
      continue;
    }

    const double start = start_times_[i];
    const double duration = durations_[i];
    double local_alpha = 1.0;
    if (duration > 0.0) {
      local_alpha = (absolute_time - start) / duration;
    } else if (absolute_time < start) {
      local_alpha = 0.0;
    }
    animation->interpolate_with_rate(std::clamp(local_alpha, 0.0, 1.0));
  }
}

void LaggedStartAnimation::finish() {
  for (Animation* animation : animations_) {
    if (animation == nullptr) {
      continue;
    }
    animation->finish();
  }
}

}  // namespace manim_cpp::animation
