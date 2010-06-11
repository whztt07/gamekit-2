/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Charlie C.

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
#ifndef _gkSkeleton_h_
#define _gkSkeleton_h_

#include "gkGameObject.h"
#include "gkSerialize.h"




// Manually controlled bone.
class gkBone
{
public:

    typedef utArray<gkBone *> BoneList;


public:

    gkBone(const gkString &name);
    ~gkBone();

    void        setParent(gkBone *bone);
    void        setOgreBone(Ogre::Bone *bone);
    void        setRestPosition(const gkTransformState &st);



    const gkTransformState  &getRest(void)      {return m_bind;}
    gkTransformState        &getPose(void)      {return m_pose;}
    gkBone                  *getParent(void)    {return m_parent;}
    const gkString          &getName(void)      {return m_name;}
    Ogre::Bone              *getOgreBone(void)  {return m_bone;} 
    BoneList                &getChildren(void)  {return m_children;}


private:

    const gkString m_name;

    // The main Ogre bone.
    Ogre::Bone *m_bone;

    // Parent manual bone
    gkBone      *m_parent;
    BoneList    m_children;

    // Ogre calculations are reltave to the rest position.
    gkTransformState m_bind;

    // The current pose matrix, calculated in gkActionChannel::evaluate
    gkTransformState m_pose;


};


// Game skeleton
class gkSkeleton : public gkGameObject
{
public:
    typedef utHashTable<gkHashedString, gkAction *>  Actions;
    typedef utHashTable<gkHashedString, gkBone *>    Bones;

public:
    gkSkeleton(gkScene *scene, const gkString &name);
    virtual ~gkSkeleton();

    void            setEntity(Ogre::Entity *ent);


    // Create a manual bone from an ogre bone
    gkBone         *createBone(const gkString &name);

    // Gets a manually controlled bone
    gkBone         *getBone(const gkHashedString &name);

    // Creates a new blank action
    gkAction       *createAction(const gkHashedString &name);

    // Gets a previously created action
    gkAction       *getAction(const gkHashedString &name);

    gkBone::BoneList &getRootBoneList(void);
    gkBone::BoneList &getBoneList(void) {return m_boneList;}


    // TODO attach gkGameObject to gkBone


    // existence testing
    GK_INLINE bool hasBone(const gkHashedString &name)        { return m_bones.find(name) != GK_NPOS; }
    GK_INLINE bool hasAction(const gkHashedString &name)      { return m_actions.find(name) != GK_NPOS; }

    gkEntity        *getController(void)        {return m_controller;}
    void             setController(gkEntity *v) {m_controller = v;}


protected:

    gkObject *clone(const gkString &name);
    void createSkeleton(void);

    Actions             m_actions;
    Bones               m_bones;
    gkBone::BoneList    m_boneList, m_rootBoneList;
    gkEntity            *m_controller;


    // internal skeleton loader
    Ogre::ManualResourceLoader *m_skelLoader;

    virtual void loadImpl(void);
    virtual void unloadImpl(void);
};


#endif//_gkSkeleton_h_
