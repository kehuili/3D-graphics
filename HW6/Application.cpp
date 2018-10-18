// Application.cpp: implementation of the Application class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CS580HW.h"
#include "Application.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Application::Application()
{
	m_pRender = NULL;		// the renderer
	m_pRenders = NULL;
	m_pUserInput = NULL;
	m_pFrameBuffer = NULL;
	m_pFrameBuffers = NULL;
}

Application::~Application()
{
	if(m_pFrameBuffer != NULL)
		delete m_pFrameBuffer;
	if (m_pFrameBuffers != NULL)
		delete m_pFrameBuffers;
	if(m_pUserInput != NULL)
		delete m_pUserInput;
}

