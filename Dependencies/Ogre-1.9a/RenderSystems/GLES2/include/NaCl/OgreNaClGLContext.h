/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2013 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#ifndef __NaClGLContext_H__
#define __NaClGLContext_H__

#include "OgreGLES2Context.h"
#include "OgreNaClWindow.h"
#include "OgreNaClGLContext.h"

namespace Ogre {
    class NaClGLSupport;

    class _OgrePrivate NaClGLContext : public GLES2Context, public pp::Graphics3DClient
    {
		private:
			const NaClGLSupport *mGLSupport;
            const NaClWindow * mWindow;
            pp::Instance* mInstance;
            pp::CompletionCallback* mSwapCallback;
            pp::Graphics3D mContext;
            unsigned int mWidth;
            unsigned int mHeight;
        public:
            NaClGLContext(const NaClWindow * window, const NaClGLSupport *glsupport, pp::Instance* instance, pp::CompletionCallback* swapCallback);
            virtual ~NaClGLContext();

			virtual void setCurrent();
            virtual void endCurrent();
            GLES2Context* clone() const;

            void swapBuffers(bool waitForVSync);

            void resize();

            /// The Graphics3DClient interface - pp::Graphics3DClient_Dev
            virtual void Graphics3DContextLost();

    };
}

#endif
