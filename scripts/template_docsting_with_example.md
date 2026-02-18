# Documentation Template with Example Section

Reference: <https://github.com/manim-cpp/manim-cpp/wiki/documentation-guidelines>

## SomeClass

A one line description of the class.

A short paragraph providing more details.

## Extended Summary

## Parameters

- `scale_factor`: The factor used for scaling.

## Returns

- `VMobject`: Returns the modified `VMobject`.

## Raises

- `std::invalid_argument`: If one element of the input list is not a `VMobject`.

## Examples

```cpp
#include "manim_cpp/scene/scene.hpp"
#include "manim_cpp/scene/scene_registry.hpp"

class AddTextWordByWordScene final : public manim_cpp::scene::Scene {
 public:
  void construct() override {
    // TODO: Add text mobject and animation calls.
    wait(0.5);
  }
};

MANIM_REGISTER_SCENE(AddTextWordByWordScene);
```

## See Also

- `Create`
- `ShowPassingFlash`
