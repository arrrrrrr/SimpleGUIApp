#pragma once

#include <string>
#include <functional>

/// <summary>
/// Creates an undo action that will revert an action that was performed by the user
/// </summary>
/// <param name="description">
/// Describes what the undo action will perform
/// </param name>
/// <param name="undo_lambda">
/// A lambda that will execute the undo action. This can be anything at all
/// </param name>
/// <returns>
/// An undo action that can be used to redo the undo
/// </returns>

class UndoAction {
public:
    UndoAction(std::wstring&& description, std::function<UndoAction()> undo_lambda) :
        m_description(description), m_lambda(undo_lambda)
    {
    }

    UndoAction() = default;
    UndoAction(const UndoAction& other) = delete;

    UndoAction(UndoAction&& other) noexcept {
        other.m_description = std::move(m_description);
        other.m_lambda = std::move(m_lambda);
    }

    ~UndoAction() {}

    const std::wstring& Description() {
        return m_description;
    }

    UndoAction Apply() {
        auto f = std::move(m_lambda);
        return f();
    }

private:
    std::wstring m_description;
    std::function<UndoAction()> m_lambda;
};
