#ifndef AKASHA_WORLD_COMMON__H
#define AKASHA_WORLD_COMMON__H

#include <LinearMath/btVector3.h>
#include "vector3d.h"

//################################################################
//定数・変数定義
#define BIT(x) (1<<(x))
namespace akasha {
	//! 重力定数
	//   static btScalar AKASHA_GRAVITY = (btScalar)9.81;
	constexpr btScalar AKASHA_GRAVITY = btScalar(9.81);

	//! 海面高さ
	static const btScalar SEA_HEIGHT = (btScalar)(-0.45);

	//! トイオブジェクトID
	typedef int ToyID;

	//! 衝突タイプ
	enum ColisionType {
		COL_TYPE_NOTHING      = 0,       //!< 非衝突
		COL_TYPE_LAND         = BIT(0),         //!< Land
		COL_TYPE_MODEL        = BIT(1),         //!< エレメントモデル
		COL_TYPE_SUB_MODEL_0  = BIT(2), // Sub Model
		COL_TYPE_TOY          = BIT(3), // Toy Object
		COL_TYPE_BOM          = BIT(4), // Explosion etc
		COL_TYPE_PIPE         = BIT(5), // PIPE
		COL_TYPE_AVATAR       = BIT(6), // AVATOR
	};



	// IrrVectorからbtVector
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
