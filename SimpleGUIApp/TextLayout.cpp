#include "TextLayout.h"

void TextLayout::Draw(std::shared_ptr<ID2D1HwndRenderTarget> render_target)
{
}

void TextLayout::Delegate(std::shared_ptr<BaseDelegant> delegant)
{
}

UndoAction TextLayout::ApplyUndoAction(UndoAction undo)
{
    return UndoAction();
}

D2D_RECT_F TextLayout::Bounds()
{
    return D2D_RECT_F();
}

D2D_POINT_2F TextLayout::Origin()
{
    return D2D_POINT_2F();
}

HRESULT TextLayout::InitDeviceIndependentResources()
{
    return E_NOTIMPL;
}

HRESULT TextLayout::InitDeviceResources(void* factory)
{
    return E_NOTIMPL;
}

void TextLayout::ReleaseDeviceResources()
{
}

void TextLayout::ReleaseDeviceIndependentResources()
{
}

BaseLayout* TextLayoutBuilder::build()
{
    return nullptr;
}
