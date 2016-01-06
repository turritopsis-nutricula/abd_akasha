#ifndef IRR_EXTENTION__H
#define IRR_EXTENTION__H

//irrlicht

// #define _IRR_STATIC_LIB_
#include "irrlicht.h"

#include "ISceneNodeAnimatorFinishing.h"
#include "COpenGLMaterialRenderer.h"


namespace irr{
	namespace scene{
		class CSceneNodeAnimatorSmokeDelete : public irr::scene::ISceneNodeAnimatorFinishing
		{
			public:

				//! �R���X�g���N�^
				CSceneNodeAnimatorSmokeDelete(irr::scene::ISceneManager* manager, irr::u32 when, irr::u32 when2);

				//! �A�j���[�V�������s
				virtual void animateNode(irr::scene::ISceneNode* node, irr::u32 timeMs);

				//! �N���[���쐬
				virtual ISceneNodeAnimator* createClone(irr::scene::ISceneNode* node, irr::scene::ISceneManager* newManager/* =0*/);

			private:
				irr::scene::ISceneManager* SceneManager;
				irr::u32 StartTime;
		};


		class COpenGLMaterialRenderer_STICKER : public irr::video::IMaterialRenderer
		{
			irr::video::COpenGLDriver* Driver;
			public:
			COpenGLMaterialRenderer_STICKER(irr::video::COpenGLDriver* d) : Driver(d){}

			virtual void OnSetMaterial(const irr::video::SMaterial& material, const irr::video::SMaterial& lastMaterial,
					bool resetAllRenderstates, irr::video::IMaterialRendererServices* services)
			{
// 				Driver->disableTextures(1);
				Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

				if((material.MaterialType != lastMaterial.MaterialType) || resetAllRenderstates)
				{
#ifdef   GL_ARB_texture_env_combine
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE_ARB);
					glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_PRIMARY_COLOR);
					glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,   GL_INTERPOLATE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,   GL_TEXTURE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,   GL_PRIMARY_COLOR);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB,   GL_TEXTURE);
					glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_SRC_ALPHA);
#else
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMVINE_EXT);
					glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_REPLACE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_PRIMARY_COLOR);
					glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT,   GL_INTERPOLATE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT,   GL_TEXTURE);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT,   GL_PRIMARY_COLOR);
					glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE2_RGB_EXT,   GL_TEXTURE);
					glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT, GL_SRC_ALPHA);
#endif
				}
			}

			virtual void OnUnsetMaterial()
			{
				if (Driver->queryFeature(irr::video::EVDF_MULTITEXTURE))
				{
					Driver->extGlActiveTexture(GL_TEXTURE1_ARB);
					glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#ifdef GL_ARB_texture_env_combine
					glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_SRC_COLOR);
#else
					glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT, GL_SRC_COLOR);
#endif
					Driver->extGlActiveTexture(GL_TEXTURE0_ARB);
				}
			}
		};
	} // irrlicht
} // akasha

#endif// IRR_EXTENTION__H
