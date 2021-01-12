#pragma once
#include "BaseLayout.h"

class TextLayout :
    public BaseLayout
{
public:
    TextLayout() = delete;

    TextLayout(const TextLayout&) = delete;
    TextLayout& operator=(const TextLayout&) = delete;

    TextLayout(TextLayout &&other);

    // Inherited via BaseLayout
    virtual void Draw(std::shared_ptr<ID2D1HwndRenderTarget> render_target) override;
    virtual void Delegate(std::shared_ptr<BaseDelegant> delegant) override;
    virtual UndoAction ApplyUndoAction(UndoAction undo) override;
    virtual D2D_RECT_F Bounds() override;
    virtual D2D_POINT_2F Origin() override;
    virtual HRESULT InitDeviceIndependentResources() override;
    virtual HRESULT InitDeviceResources(void* factory) override;
    virtual void ReleaseDeviceResources() override;
    virtual void ReleaseDeviceIndependentResources() override;

protected:
    friend class TextLayoutBuilder;

private:

};

class TextLayoutBuilder final :
    public BaseLayoutBuilder
{
public:
    using BaseLayoutBuilder::BaseLayoutBuilder;

    // Inherited via BaseLayoutBuilder
    virtual BaseLayout* build() override;

private:

};

