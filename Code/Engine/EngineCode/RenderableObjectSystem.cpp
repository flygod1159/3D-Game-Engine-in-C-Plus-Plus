
#include "PreCompiled.h"
#include "HashedString.h"
#include "Material.h"
#include "MeshData.h"
#include "RenderableObjectSystem.h"
#include "GraphicsSystem.h"
#include "Vector3.h"
#include "HashedString.h"
#include "DebugLineRenderer.h"

namespace Engine
{
	MemoryPool *RenderableObjectSystem::Renderable3DObject::Renderer3DMemoryPool = NULL;
	MemoryPool *RenderableObjectSystem::RenderableSprites::SpriteMemoryPool = NULL;
	unsigned int RenderableObjectSystem::MAX_3D_OBJECTS = 100;
	unsigned int RenderableObjectSystem::MAX_2D_OBJECTS = 100;
	RenderableObjectSystem* RenderableObjectSystem::mInstance = NULL;

	/***************************3D Rendering **************************************/
	/******************************************************************************
		Function     : Renderable3DObject
		Description  : Constructor of 3D Rendering object 
		Input        : SharedPointer<Actor> &i_WorldObject, 
					SharedPointer<Cheesy::Material> &i_Material,
					SharedPointer<Cheesy::Mesh> &i_Mesh
		Output       : 
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/	
	RenderableObjectSystem::Renderable3DObject::Renderable3DObject
	(
		SharedPointer<Actor> &i_WorldObject, 
		SharedPointer<Material> &i_Material,
		SharedPointer<Mesh> &i_Mesh
	):
	m_WorldObject(i_WorldObject),
	mMaterial(i_Material),
	mMesh(i_Mesh)
	{
			
	}

	/******************************************************************************
		Function     : ~Renderable3DObject
		Description  : Destructor of 3D Rendering object 
		Input        : 
		Output       : 
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	RenderableObjectSystem::Renderable3DObject::~Renderable3DObject()
	{

	}

	/******************************************************************************
		Function     : GetMaterial
		Description  : Function to get material of 3D object for rendering
		Input        : void
		Output       : shared material pointer for rendering
		Return Value : SharedPointer<Material> 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	SharedPointer<Material> RenderableObjectSystem::Renderable3DObject::GetMaterial(void) const
	{
		return mMaterial;
	}

	/******************************************************************************
		Function     : GetMesh
		Description  : Function to get mesh of 3D object for rendering
		Input        : void
		Output       : shared mesh pointer for rendering
		Return Value : SharedPointer<Mesh> 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	SharedPointer<Mesh> RenderableObjectSystem::Renderable3DObject::GetMesh(void) const
	{
		return mMesh;
	}

	/***************************Sprite Rendering **************************************/
	RenderableObjectSystem::RenderableSprites::RenderableSprites(SharedPointer<Sprite> &i_Sprite):
		mSprite(i_Sprite)
	{

	}

	RenderableObjectSystem::RenderableSprites::~RenderableSprites()
	{

	}

	SharedPointer<Sprite> RenderableObjectSystem::RenderableSprites::GetSprite(void) const
	{
		return mSprite;
	}

	/***************************Global Rendering***********************************/

	/******************************************************************************
	Function     : Add3DActorGameObject
	Description  : Function to add 3D Actor object for Rendering, Only creates
	Material and Mesh if not in cache
	Input        : void
	Output       :
	Return Value :

	History      :
	Author       : Vinod VM
	Modification : Created function
	******************************************************************************/
	void RenderableObjectSystem::Add3DActorGameObject(
		SharedPointer<Actor> &i_Object,
		const char *pcMaterialPath,
		const char *pcMeshPath)
	{
		assert(pcMaterialPath);

		SharedPointer<Material> NewMaterial;
		{
			//create new material and insert into material cache eventually
			NewMaterial = GraphicsSystem::GetInstance()->CreateMaterial(pcMaterialPath);

			//Set color constant from fragment shader
			//NewMaterial->SetFragmentShaderConstantValue(Vector3(3.0f, 6.0f, 2.0f));
		}

		if (NewMaterial == NULL)
		{
			return;
		}

		SharedPointer<Mesh> NewMesh;
		{
			//create new mesh and insert into mesh cache eventually
			NewMesh = GraphicsSystem::GetInstance()->CreateMesh(pcMeshPath);
		}

		if (NewMesh == NULL)
		{
			return;
		}

		m3DRenderableObjects.push_back(new Renderable3DObject(i_Object, NewMaterial, NewMesh));
	}

	void RenderableObjectSystem::CreateSprite(const char* i_TexturePath, const sRectangle &i_texcoordsRect, const float left, const float top, const float width,
											unsigned int i_horizontalSpriteCount, unsigned int i_verticalSpriteCount)
	{
		assert(i_TexturePath);

		SharedPointer<Sprite> NewSprite;
		{
			sSprite spriteDetails(left, top, width, i_horizontalSpriteCount, i_verticalSpriteCount);

			//create new mesh and insert into mesh cache eventually
			NewSprite = GraphicsSystem::GetInstance()->CreateSprite(i_TexturePath, i_texcoordsRect, spriteDetails);
		}

		if (NewSprite == NULL)
		{
			return;
		}

		mSpriteRenderableObjects.push_back(new RenderableSprites(NewSprite));
	}

	/******************************************************************************
		Function     : DeleteMarkedToDeathGameObjects
		Description  : Function to delete marked to death actor game object from 
					rendering system
		Input        : void
		Output       : void
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void RenderableObjectSystem::DeleteMarkedToDeathGameObjects(void)
	{
		for (unsigned long ulCount = 0; ulCount < m3DRenderableObjects.size(); )
		{
			if( m3DRenderableObjects.at(ulCount)->m_WorldObject->IsMarkedForDeath() )
			{
				delete (m3DRenderableObjects.at(ulCount));
				m3DRenderableObjects.erase(m3DRenderableObjects.begin() + ulCount);
			}
			else
			{
				ulCount++;
			}
		}
	}

	/******************************************************************************
		Function     : DeleteAllRenderableObjects
		Description  : Function to delete all renderable object from 
					rendering system
		Input        : void
		Output       : void
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void RenderableObjectSystem::DeleteAllRenderableObjects(void)
	{
		//Delete 3D objects
		for (unsigned long ulCount = 0; ulCount < m3DRenderableObjects.size(); ulCount++)
		{
			delete m3DRenderableObjects.at(ulCount);
		}

		m3DRenderableObjects.clear();

		//Delete 3D objects
		for (unsigned long ulCount = 0; ulCount < mSpriteRenderableObjects.size(); ulCount++)
		{
			delete mSpriteRenderableObjects.at(ulCount);
		}

		mSpriteRenderableObjects.clear();

	}

	bool RenderableObjectSystem::Delete3DGameObjectByName(const char * iName)
	{
		//Delete 3D objects
		for (unsigned long ulCount = 0; ulCount < m3DRenderableObjects.size(); ulCount++)
		{
			if (m3DRenderableObjects.at(ulCount)->m_WorldObject->mHashedName.Get() == HashedString::Hash(iName))
			{
				delete m3DRenderableObjects.at(ulCount);
				return true;
			}
		}

		return false;
	}

	bool RenderableObjectSystem::DeleteSpriteGameObjectByName(const char * iName)
	{

		//Delete Sprite
		for (unsigned long ulCount = 0; ulCount < mSpriteRenderableObjects.size(); ulCount++)
		{
			if (mSpriteRenderableObjects.at(ulCount)->GetSprite()->GetName() == (iName))
			{
				delete mSpriteRenderableObjects.at(ulCount);
				return true;
			}
		}

		return false;
	}

	RenderableObjectSystem::RenderableSprites * RenderableObjectSystem::FindSpriteGameObjectByName(const char * iName)
	{

		//Find Sprite
		for (unsigned long ulCount = 0; ulCount < mSpriteRenderableObjects.size(); ulCount++)
		{
			if (mSpriteRenderableObjects.at(ulCount)->GetSprite()->GetName() == (iName))
			{
				return mSpriteRenderableObjects.at(ulCount);
			}
		}

		return NULL;
	}

	bool RenderableObjectSystem::CreateDebugLines(const char * iName, const unsigned int iMaxlines)
	{
#ifdef EAE2014_DEBUGLINE_SHOULDDRAW
		assert(GraphicsSystem::GetInstance());

		if (!GraphicsSystem::GetInstance()->CreateDebugLineRenderer(iName, iMaxlines))
		{
			return false;
		}
#endif
		return true;
	}

	void RenderableObjectSystem::AddDebugLines(const sLine &iLine)
	{
#ifdef EAE2014_DEBUGLINE_SHOULDDRAW
		assert(DebugLineRenderer::GetInstance());

		DebugLineRenderer::GetInstance()->AddLines(iLine);
#endif
	}

	/******************************************************************************
		Function     : Render
		Description  : Function to render #D, 2D, Text objects
		Input        : 
		Output       : 
		Return Value : 

		History      :
		Author       : Vinod VM
		Modification : Created function
	******************************************************************************/
	void RenderableObjectSystem::Render()
	{
		Vector3  CurrentPosition;
		Vector3 CurrentVelocity;
		Vector3 CurrentAcceleration;
		Vector3 CurrentFriction;
		
		DeleteMarkedToDeathGameObjects();
#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
		D3DPERF_BeginEvent(0, L"Mesh Drawing");
#endif
		GraphicsSystem::GetInstance()->BeingFrame();
		

		if (GraphicsSystem::GetInstance()->Begin3D())
		{
			//Render Logic
			for (unsigned long ulCount = 0; ulCount < m3DRenderableObjects.size(); ulCount++)
			{
				GraphicsSystem::GetInstance()->Render(m3DRenderableObjects.at(ulCount)->GetMaterial(), m3DRenderableObjects.at(ulCount)->GetMesh(), m3DRenderableObjects[ulCount]->m_WorldObject);
			}
#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
			D3DPERF_EndEvent();
#endif
#ifdef EAE2014_DEBUGLINE_SHOULDDRAW
#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
			D3DPERF_BeginEvent(0, L"DebugLine Drawing");
#endif
			DebugLineRenderer::GetInstance()->Render();
#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
			D3DPERF_EndEvent();
#endif
#endif
		}

#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
		D3DPERF_BeginEvent(0, L"Sprite Drawing");
#endif
		if (GraphicsSystem::GetInstance()->Begin2D())
		{
			//Render sprites
			for (unsigned long ulCount = 0; ulCount < mSpriteRenderableObjects.size(); ulCount++)
			{
				GraphicsSystem::GetInstance()->RenderSprite(mSpriteRenderableObjects.at(ulCount)->GetSprite());
			}
		}
#ifdef EAE2014_GRAPHICS_AREPIXEVENTSENABLED
		D3DPERF_EndEvent();
#endif
		GraphicsSystem::GetInstance()->EndFrame();
		return;
	}

	RenderableObjectSystem::RenderableObjectSystem()
	{
		bool WereThereErrors = false;

		if (Renderable3DObject::Renderer3DMemoryPool == NULL)
		{
			Renderable3DObject::Renderer3DMemoryPool = MemoryPool::Create(sizeof(Renderable3DObject), MAX_3D_OBJECTS);
			if (Renderable3DObject::Renderer3DMemoryPool == NULL)
			{
				assert(false);
				WereThereErrors = true;
			}
		}

		if (RenderableSprites::SpriteMemoryPool == NULL)
		{
			RenderableSprites::SpriteMemoryPool = MemoryPool::Create(sizeof(RenderableSprites), MAX_2D_OBJECTS);
			if (RenderableSprites::SpriteMemoryPool == NULL)
			{
				assert(false);
				WereThereErrors = true;
			}
		}

		mInitilized = !WereThereErrors;
	}

	RenderableObjectSystem::~RenderableObjectSystem()
	{
		DeleteAllRenderableObjects();
		m3DRenderableObjects.clear();
		mSpriteRenderableObjects.clear();

		if (Renderable3DObject::Renderer3DMemoryPool)
		{
			unsigned long ulOutLength;
			Renderable3DObject::Renderer3DMemoryPool->Destroy(&ulOutLength);
		}

		if (RenderableSprites::SpriteMemoryPool)
		{
			unsigned long ulOutLength;
			RenderableSprites::SpriteMemoryPool->Destroy(&ulOutLength);
		}
	}

	bool RenderableObjectSystem::CreateInstance(const HWND i_mainWindow)
	{
		if (mInstance == NULL)
		{
			if (false == GraphicsSystem::CreateInstance(i_mainWindow))
			{
				assert(false);
				return NULL;
			}

			mInstance = new RenderableObjectSystem();

			if (mInstance == NULL)
			{
				return false;
			}

			if (mInstance->mInitilized == false)
			{
				delete mInstance;
				return false;
			}
		}

		return true;
	}

	RenderableObjectSystem * RenderableObjectSystem::GetInstance()
	{
		if (mInstance != NULL)
		{
			return mInstance;
		}

		assert(false);

		return NULL;
	}

	void RenderableObjectSystem::Destroy()
	{
		if (mInstance)
		{
			GraphicsSystem::Destroy();
			delete mInstance;

			return;
		}

		assert(false);
	}
}