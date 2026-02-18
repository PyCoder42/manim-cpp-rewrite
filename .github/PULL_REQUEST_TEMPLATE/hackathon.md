Thanks for your contribution to the manim-cpp hackathon track.

Please make sure your pull request has a meaningful title.

E.g. "Example for the class Angle".

Submission details are posted in the project coordination channel.

Use this style when sharing an API/documentation sketch:
```
/// Brief summary.
/// Parameters:
/// - scene: active scene context.
```
```cpp
class HelloWorld final : public manim_cpp::scene::Scene {
 public:
  void construct() override { add(std::make_shared<manim_cpp::mobject::Circle>()); }
};
```
Copy or adapt the snippet into the relevant source/docs location.

If you need help, ask in the project collaboration channel.

## Checklist
- [ ] Milestone gate review completed for the current plan tranche.
