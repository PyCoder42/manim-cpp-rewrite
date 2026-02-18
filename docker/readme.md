See the [main README](../README.md) for usage and build guidance.

# Building the image
The docker image corresponding to the checked out version of the git repository
can be built by running
```bash
docker build -t your-namespace/manim-cpp:TAG -f docker/Dockerfile .
```
from the root directory of the repository.

Multi-platform builds are possible by running
```bash
docker buildx build --push --platform linux/arm64/v8,linux/amd64 --tag your-namespace/manim-cpp:TAG -f docker/Dockerfile .
```
from the root directory of the repository.
