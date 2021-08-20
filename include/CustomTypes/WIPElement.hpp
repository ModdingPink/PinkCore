#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Sprite.hpp"

DECLARE_CLASS_CODEGEN(PinkCore::UI, WIPElement, UnityEngine::MonoBehaviour,
    DECLARE_CTOR(ctor);
    DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, sprite);

    public:
        bool inited = false;
        void Init();
        void CheckWipState();
)
