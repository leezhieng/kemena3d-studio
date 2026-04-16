#include "commands.h"
#include "manager.h"

#include <kemena/kmesh.h>
#include <kemena/klight.h>

// ---------------------------------------------------------------------------
// UndoRedoManager
// ---------------------------------------------------------------------------

void UndoRedoManager::push(std::unique_ptr<ICommand> cmd)
{
    redoStack.clear();
    undoStack.push_back(std::move(cmd));
    if (undoStack.size() > MaxHistory)
        undoStack.pop_front();
}

void UndoRedoManager::undo()
{
    if (undoStack.empty()) return;
    auto &cmd = undoStack.back();
    cmd->undo();
    redoStack.push_back(std::move(cmd));
    undoStack.pop_back();
}

void UndoRedoManager::redo()
{
    if (redoStack.empty()) return;
    auto &cmd = redoStack.back();
    cmd->redo();
    undoStack.push_back(std::move(cmd));
    redoStack.pop_back();
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void applyTransformStates(const std::vector<TransformState> &states, Manager *mgr)
{
    for (const auto &s : states)
    {
        kObject *obj = mgr->findObjectByUuid(s.uuid);
        if (!obj) continue;
        obj->setPosition(s.pos);
        obj->setRotation(s.rot);
        obj->setScale(s.scale);
    }
}

// ---------------------------------------------------------------------------
// TransformCommand
// ---------------------------------------------------------------------------

void TransformCommand::undo()
{
    applyTransformStates(before, manager);
}

void TransformCommand::redo()
{
    applyTransformStates(after, manager);
}

// ---------------------------------------------------------------------------
// DeleteCommand
// ---------------------------------------------------------------------------

DeleteCommand::~DeleteCommand()
{
    if (ownsObjects)
    {
        for (auto &info : deleted)
            delete info.object;
    }
}

void DeleteCommand::undo()
{
    // Re-add each object to its scene
    for (auto &info : deleted)
    {
        if (!info.scene || !info.object) continue;

        if (info.type == NODE_TYPE_LIGHT)
            info.scene->addLight(static_cast<kLight *>(info.object));
        else
            info.scene->addMesh(static_cast<kMesh *>(info.object), info.object->getUuid());
    }

    ownsObjects = false;

    // Restore selection
    manager->selectedObjects   = selectionBefore;
    manager->selectedObject    = selectedObjBefore;

    if (manager->panelHierarchy)
        manager->panelHierarchy->refreshList();
}

void DeleteCommand::redo()
{
    for (auto &info : deleted)
    {
        if (!info.scene || !info.object) continue;

        if (info.type == NODE_TYPE_LIGHT)
            info.scene->removeLight(static_cast<kLight *>(info.object));
        else
            info.scene->removeMesh(static_cast<kMesh *>(info.object));
    }

    ownsObjects = true;

    // Clear selection
    manager->selectedObjects.clear();
    manager->selectedObject = nullptr;

    if (manager->panelHierarchy)
        manager->panelHierarchy->refreshList();
}

// ---------------------------------------------------------------------------
// SelectCommand
// ---------------------------------------------------------------------------

void SelectCommand::undo()
{
    manager->selectedObjects = before;
    manager->selectedObject  = selectedObjBefore;
}

void SelectCommand::redo()
{
    manager->selectedObjects = after;
    manager->selectedObject  = selectedObjAfter;
}
