#ifndef COMMANDS_H
#define COMMANDS_H

#include <functional>
#include <memory>
#include <deque>
#include <vector>

#include <kemena/kdatatype.h>
#include <kemena/kobject.h>
#include <kemena/kscene.h>

using namespace kemena;

class Manager;  // forward declaration — full definition in manager.h

// ---------------------------------------------------------------------------
// Pivot mode for multi-object gizmo
// ---------------------------------------------------------------------------
enum class PivotMode
{
    Individual,   // each object transforms around its own centre
    Center,       // transforms around the centroid of all selected objects
    LastSelected  // transforms around the last-selected object's pivot
};

// ---------------------------------------------------------------------------
// Per-object transform snapshot (used by TransformCommand)
// ---------------------------------------------------------------------------
struct TransformState
{
    kString uuid;
    kVec3   pos;
    kQuat   rot;
    kVec3   scale;
};

// ---------------------------------------------------------------------------
// Base command interface
// ---------------------------------------------------------------------------
struct ICommand
{
    virtual ~ICommand() = default;
    virtual void undo() = 0;
    virtual void redo() = 0;
};

// ---------------------------------------------------------------------------
// Undo / Redo manager
// ---------------------------------------------------------------------------
class UndoRedoManager
{
public:
    static constexpr size_t MaxHistory = 100;

    void push(std::unique_ptr<ICommand> cmd); // clears redo stack, trims history
    void undo();
    void redo();
    bool canUndo() const { return !undoStack.empty(); }
    bool canRedo() const { return !redoStack.empty(); }
    void clear()         { undoStack.clear(); redoStack.clear(); }

private:
    std::deque<std::unique_ptr<ICommand>> undoStack;
    std::deque<std::unique_ptr<ICommand>> redoStack;
};

// ---------------------------------------------------------------------------
// PropertyCommand — generic lambda pair, no Manager access needed
// ---------------------------------------------------------------------------
struct PropertyCommand : ICommand
{
    std::function<void()> undoFn;
    std::function<void()> redoFn;

    PropertyCommand(std::function<void()> u, std::function<void()> r)
        : undoFn(std::move(u)), redoFn(std::move(r)) {}

    void undo() override { undoFn(); }
    void redo() override { redoFn(); }
};

// ---------------------------------------------------------------------------
// TransformCommand — gizmo / inspector drag that may span several objects
// ---------------------------------------------------------------------------
struct TransformCommand : ICommand
{
    std::vector<TransformState> before;
    std::vector<TransformState> after;
    Manager *manager;

    TransformCommand(Manager *mgr,
                     std::vector<TransformState> b,
                     std::vector<TransformState> a)
        : manager(mgr), before(std::move(b)), after(std::move(a)) {}

    void undo() override;
    void redo() override;
};

// ---------------------------------------------------------------------------
// DeleteCommand — stores detached objects so delete is undoable
// ---------------------------------------------------------------------------
struct DeletedObjectInfo
{
    kObject  *object = nullptr;
    kNodeType type   = NODE_TYPE_OBJECT;
    kScene   *scene  = nullptr;
};

struct DeleteCommand : ICommand
{
    std::vector<DeletedObjectInfo> deleted;
    std::vector<kString>           selectionBefore;   // UUIDs before deletion
    kObject                       *selectedObjBefore = nullptr;
    Manager                       *manager;
    bool                           ownsObjects = true; // true → destructor frees memory

    DeleteCommand(Manager                       *mgr,
                  std::vector<DeletedObjectInfo> del,
                  std::vector<kString>           selBefore,
                  kObject                       *selObjBefore)
        : manager(mgr)
        , deleted(std::move(del))
        , selectionBefore(std::move(selBefore))
        , selectedObjBefore(selObjBefore) {}

    ~DeleteCommand() override;
    void undo() override;
    void redo() override;
};

// ---------------------------------------------------------------------------
// SelectCommand — records a selection change for undo/redo
// ---------------------------------------------------------------------------
struct SelectCommand : ICommand
{
    std::vector<kString> before;
    std::vector<kString> after;
    kObject             *selectedObjBefore = nullptr;
    kObject             *selectedObjAfter  = nullptr;
    Manager             *manager;

    SelectCommand(Manager            *mgr,
                  std::vector<kString> b, kObject *selB,
                  std::vector<kString> a, kObject *selA)
        : manager(mgr)
        , before(std::move(b)), selectedObjBefore(selB)
        , after(std::move(a)),  selectedObjAfter(selA) {}

    void undo() override;
    void redo() override;
};

#endif // COMMANDS_H
