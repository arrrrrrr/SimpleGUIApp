#pragma once
#include "common.h"
#include "UndoAction.h"

class BaseDelegant;

class BaseLayout
{
public:
    BaseLayout() = default;

    virtual ~BaseLayout() {
        ReleaseDeviceResources();
        ReleaseDeviceIndependentResources();
    }

    BaseLayout(const BaseLayout&) = delete;
    BaseLayout& operator=(const BaseLayout&) = delete;

    BaseLayout(BaseLayout&& bb) noexcept {}

    // Instance call dispatcher to initialize device dependent resources
    static HRESULT cbInitDeviceResources(BaseLayout *bb, void *factory) {
        return bb->InitDeviceResources(factory);
    }

    // Instance call dispatcher to release device dependent resources
    static void cbReleaseDeviceResources(BaseLayout* bb) {
        bb->ReleaseDeviceResources();
    }

    virtual void Draw(std::shared_ptr<ID2D1HwndRenderTarget> render_target) = 0;
    virtual void Delegate(std::shared_ptr<BaseDelegant> delegant) = 0;

    // Returns an undo action that can be used bool whether there are anymore actions to undo
    // On the final action the layout will call its destructor
    virtual UndoAction ApplyUndoAction(UndoAction undo) = 0;

    virtual D2D_RECT_F Bounds() = 0;
    virtual D2D_POINT_2F Origin() = 0;

protected:
    virtual HRESULT InitDeviceIndependentResources() = 0;
    virtual HRESULT InitDeviceResources(void *factory) = 0;
    virtual void ReleaseDeviceResources() = 0;
    virtual void ReleaseDeviceIndependentResources() = 0;
};

// BaseLayout Builder to build layouts
class BaseLayoutBuilder {
public:
    BaseLayoutBuilder() = default;
    BaseLayoutBuilder(const BaseLayoutBuilder&) = delete;
    BaseLayoutBuilder& operator=(const BaseLayoutBuilder&) = delete;
    BaseLayoutBuilder(BaseLayoutBuilder&&) = delete;

    virtual BaseLayout* build() = 0;
};


