/*
Copyright 2019 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

#ifndef SVGViewer_WindrawSVGRenderer_h
#define SVGViewer_WindrawSVGRenderer_h

#include "SVGRenderer.h"
#include "wdl.h"
#include <memory>
#include <stack>

namespace SVGNative
{
class WindrawSVGPath final : public Path
{
public:
    WindrawSVGPath();
    ~WindrawSVGPath();

    void Rect(float x, float y, float width, float height) override;
    void RoundedRect(float x, float y, float width, float height, float cornerRadius) override;
    void Ellipse(float cx, float cy, float rx, float ry) override;

    void MoveTo(float x, float y) override;
    void LineTo(float x, float y) override;
    void CurveTo(float x1, float y1, float x2, float y2, float x3, float y3) override;
    void CurveToV(float x2, float y2, float x3, float y3) override;
    void ClosePath() override;

    const WD_HPATH Path();
private:
    void ClosePathSink();

    float mCurrentX{};
    float mCurrentY{};

    WD_HPATH mPath{};
    WD_PATHSINK mSink{};
    bool mHasOpenPathSink{};
    bool mHasOpenFigure{};
};

class WindrawSVGTransform final : public Transform
{
public:
    WindrawSVGTransform(float a, float b, float c, float d, float tx, float ty);

    void Set(float a, float b, float c, float d, float tx, float ty) override;
    void Rotate(float r) override;
    void Translate(float tx, float ty) override;
    void Scale(float sx, float sy) override;
    void Concat(const Transform& other) override;

    void Multiply(const WD_MATRIX& o);

    WD_MATRIX mTransform{};
};

class WindrawSVGImageData final : public ImageData
{
public:
    WindrawSVGImageData(const std::string& base64, ImageEncoding encoding);

    float Width() const override;

    float Height() const override;
};

class WindrawSVGRenderer final : public SVGRenderer
{
public:
    WindrawSVGRenderer();

    std::unique_ptr<ImageData> CreateImageData(const std::string& base64, ImageEncoding encoding) override { return std::unique_ptr<WindrawSVGImageData>(new WindrawSVGImageData(base64, encoding)); }

    std::unique_ptr<Path> CreatePath() override { return std::unique_ptr<WindrawSVGPath>(new WindrawSVGPath); }

    std::unique_ptr<Transform> CreateTransform(
        float a = 1.0, float b = 0.0, float c = 0.0, float d = 1.0, float tx = 0.0, float ty = 0.0) override
    {
        return std::unique_ptr<WindrawSVGTransform>(new WindrawSVGTransform(a, b, c, d, tx, ty));
    }

    void Save(const GraphicStyle& graphicStyle) override;
    void Restore() override;

    void DrawPath(const Path& path, const GraphicStyle& graphicStyle, const FillStyle& fillStyle, const StrokeStyle& strokeStyle) override;
    void DrawImage(const ImageData& image, const GraphicStyle& graphicStyle, const Rect& clipArea, const Rect& fillArea) override;

    void SetCanvas(WD_HCANVAS canvas)
    {
        mCanvas = canvas;
    }
private:
    WD_HBRUSH CreateBrush(const Paint& paint);

    WD_HCANVAS mCanvas{};
    std::stack<std::shared_ptr<WindrawSVGTransform>> mTransforms;
    std::stack<float> mOpacities;
};

} // namespace SVGNative

#endif // SVGViewer_WindrawSVGRenderer_h
