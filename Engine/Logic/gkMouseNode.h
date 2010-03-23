/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

    Contributor(s): Nestor Silveira.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#ifndef _gkMouseNode_h_
#define _gkMouseNode_h_

#include "gkLogicNode.h"
#include "gkInput.h"

class gkMouseNode : public gkLogicNode
{
public:
	
	enum
	{
		SCALE_X,
		SCALE_Y,
		MOTION,
		REL_X,
		REL_Y,
		ABS_X,
		ABS_Y,
		WHEEL
	};

	DECLARE_SOCKET_TYPE(SCALE_X, gkScalar);
	DECLARE_SOCKET_TYPE(SCALE_Y, gkScalar);
	DECLARE_SOCKET_TYPE(MOTION, bool);
	DECLARE_SOCKET_TYPE(REL_X, gkScalar);
	DECLARE_SOCKET_TYPE(REL_Y, gkScalar);
	DECLARE_SOCKET_TYPE(ABS_X, gkScalar);
	DECLARE_SOCKET_TYPE(ABS_Y, gkScalar);
	DECLARE_SOCKET_TYPE(WHEEL, gkScalar);

    gkMouseNode(gkLogicTree *parent, size_t id);

    virtual ~gkMouseNode() {}

    bool evaluate(gkScalar tick);

    GK_INLINE gkLogicSocket<gkScalar>* getScaleX() {return GET_SOCKET(SCALE_X);}
    GK_INLINE gkLogicSocket<gkScalar>* getScaleY() {return GET_SOCKET(SCALE_Y);}
    GK_INLINE gkLogicSocket<bool>* getMotion() {return GET_SOCKET(MOTION);}
    GK_INLINE gkLogicSocket<gkScalar>* getRelX() {return GET_SOCKET(REL_X);}
    GK_INLINE gkLogicSocket<gkScalar>* getRelY() {return GET_SOCKET(REL_Y);}
    GK_INLINE gkLogicSocket<gkScalar>* getAbsX() {return GET_SOCKET(ABS_X);}
    GK_INLINE gkLogicSocket<gkScalar>* getAbsY() {return GET_SOCKET(ABS_Y);}
    GK_INLINE gkLogicSocket<gkScalar>* getWheel() {return GET_SOCKET(WHEEL);}
};

#endif//_gkMouseNode_h_
