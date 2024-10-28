#include "Component.h"
#include "GameObject.h"

bool Component::IsActive() const {
    return _active && _owner && _owner->IsActive();
}