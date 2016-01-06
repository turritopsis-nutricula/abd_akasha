#ifndef AKASHA_WORLD_COMMON__H
#define AKASHA_WORLD_COMMON__H

#include <LinearMath/btVector3.h>
#include "vector3d.h"

//################################################################
//�萔�E�ϐ���`
#define BIT(x) (1<<(x))
namespace akasha {
	//! �d�͒萔
	//   static btScalar AKASHA_GRAVITY = (btScalar)9.81;
	constexpr btScalar AKASHA_GRAVITY = btScalar(9.81);

	//! �C�ʍ���
	static const btScalar SEA_HEIGHT = (btScalar)(-0.45);

	//! �g�C�I�u�W�F�N�gID
	typedef int ToyID;

	//! �Փ˃^�C�v
	enum ColisionType {
		COL_TYPE_NOTHING      = 0,       //!< ��Փ�
		COL_TYPE_LAND         = BIT(0),         //!< Land
		COL_TYPE_MODEL        = BIT(1),         //!< �G�������g���f��
		COL_TYPE_SUB_MODEL_0  = BIT(2), // Sub Model
		COL_TYPE_TOY          = BIT(3), // Toy Object
		COL_TYPE_BOM          = BIT(4), // Explosion etc
		COL_TYPE_PIPE         = BIT(5), // PIPE
		COL_TYPE_AVATAR       = BIT(6), // AVATOR
	};



	// IrrVector����btVector
	static inline void IrrVectorConvert(
			const irr::core::vector3df& irrVec,
			btVector3& outVec)
	{
		outVec.setX(static_cast<btScalar>(irrVec.X));
		outVec.setY(static_cast<btScalar>(irrVec.Y));
		outVec.setZ(static_cast<btScalar>(irrVec.Z));
	};

};

#undef BIT



#endif //AKASHA_WORLD_COMMON__H
