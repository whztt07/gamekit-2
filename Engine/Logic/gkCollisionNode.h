/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Nestor Silveira.

    Contributor(s): none yet.
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
#ifndef _gkCollisionNode_h_
#define _gkCollisionNode_h_

#include "gkLogicNode.h"

class gkCollisionNode : public gkLogicNode
{
public:
	enum 
	{
		ENABLE,
		TARGET,
		COLLIDES_WITH,
		HAS_COLLIDED,
		CONTACT_POSITION,
		MAX_SOCKETS
	};

	gkCollisionNode(gkLogicTree *parent, size_t id);

	~gkCollisionNode();

	void update(Ogre::Real tick);

	bool evaluate(Ogre::Real tick);

	GK_INLINE gkLogicSocket* getEnable() {return &m_sockets[ENABLE];}
	GK_INLINE gkLogicSocket* getTarget() {return &m_sockets[TARGET];}
	GK_INLINE gkLogicSocket* getCollidesWith() {return &m_sockets[COLLIDES_WITH];}
	GK_INLINE gkLogicSocket* getHasCollided() {return &m_sockets[HAS_COLLIDED];}
	GK_INLINE gkLogicSocket* getContactPosition() {return &m_sockets[CONTACT_POSITION];}

private:

	gkLogicSocket m_sockets[MAX_SOCKETS];

	gkRigidBody* m_bBody;
};

#endif//_gkCollisionNode_h_