#pragma once
#include "stdafx.h"

namespace Offender {

    class AlienShip : public Mesh {
        private:
            void            InitGeometry();
        public:
            AlienShip();
            GLboolean       Draw();
    };

} // namespace Offender
