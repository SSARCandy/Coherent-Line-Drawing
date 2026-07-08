# Coherent Line Drawing — WebGL

A GPU port of the C++/OpenCV implementation, running entirely in the browser with WebGL2 fragment shaders. Zero dependencies, single `index.html`.

## Usage

Open `index.html` directly in a browser (works over `file://`), or serve it:

```sh
python -m http.server 8000
# http://localhost:8000/webgl/
```

Drag & drop an image (or use **Open image**), tune parameters, and everything re-runs in real time. Query params are supported for quick demos: `index.html?img=../data/Einstein.jpg&view=2`.

## Pipeline (one fragment-shader pass each)

| Stage | C++ counterpart | Shader |
|---|---|---|
| Grayscale + min-max normalize | `initial_ETF` normalize | `FS_GRAY` / `FS_STRETCH` |
| 5×5 Sobel gradient | `cv::Sobel(ksize=5)` | `FS_SOBEL` |
| Initial ETF (gradient ⟂) | `initial_ETF` | `FS_ETF_INIT` |
| ETF refinement, Eq.(1)–(5) | `refine_ETF` | `FS_ETF_REFINE` (ping-pong) |
| DoG across gradient | `gradientDoG` | `FS_DOG` |
| Flow-based DoG along ETF | `flowDoG` | `FS_FDOG` |
| Normalize + threshold | `binaryThresholding` | `FS_THRESH` |
| Edge superimposition (CLD iter) | `combineImage` | `FS_COMBINE` / `FS_BLUR3` |
| ETF visualization | `postprocess::visualizeETF` | `FS_LIC` |

Global min/max normalization (`cv::NORM_MINMAX`) is done with a GPU 2×-downsample min/max reduction (`FS_REDUCE*`), reading back a single pixel.


