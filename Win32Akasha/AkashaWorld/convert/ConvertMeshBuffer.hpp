#ifndef CONVERT_MESHBUFFER__HPP
#define CONVERT_MESHBUFFER__HPP

#include <boost/scoped_array.hpp>
#include <ConvexDecomposition/ConvexDecomposition.h>
#include "ConvexDecomposition/ConvexBuilder.h"
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>

#include <IMeshBuffer.h>
namespace akasha
{
namespace convert
{

//変換用
//I : Indeisの型
//V : Vertexesの型
//TODO : rename: m_vertexesCont -> m_traiangleCount
template<typename V = float,typename I = unsigned int>
	struct MeshBuffer
{
	unsigned int m_vertexesCount, m_indexCount;
	boost::scoped_array<V> m_vertexs;
	boost::scoped_array<I> m_indices;

	//! コンストラクタ

	//irr::IMeshBufferから生成

	MeshBuffer( const irr::scene::IMeshBuffer& irrBuf ) :
		m_vertexesCount( irrBuf.getVertexCount() ),
		m_indexCount(irrBuf.getIndexCount()),
		m_vertexs( new V[irrBuf.getVertexCount()*3] ),
		m_indices( new I[irrBuf.getIndexCount()] )
	{
		for (unsigned int i=0; i< m_indexCount; i++)
		{
			m_indices[i] = static_cast<I>(irrBuf.getIndices()[i]);
		}

		//switch(irr::video::E_VERTEX_TYPE t = irrBuf.getVertexType())
		switch(irrBuf.getVertexType())
		{
		case irr::video::EVT_STANDARD:
			irrConvert<irr::video::S3DVertex>(irrBuf.getVertices());
			return;
		case irr::video::EVT_2TCOORDS:
			irrConvert<irr::video::S3DVertex2TCoords>(irrBuf.getVertices());
			return;
		case irr::video::EVT_TANGENTS:
			irrConvert<irr::video::S3DVertexTangents>(irrBuf.getVertices());
			return;
		}
	}

	//右辺コピー
	MeshBuffer(MeshBuffer<V,I>&& o) :
		m_vertexesCount(o.m_vertexesCount),
		m_indexCount( o.m_indexCount),
		m_vertexs( NULL ),
		m_indices( NULL )
	{
		boost::swap(m_vertexs, o.m_vertexs);
		boost::swap(m_indices, o.m_indices);
	}
	//コピー
	MeshBuffer(const MeshBuffer<V,I>& o) :
		m_vertexesCount(o.m_vertexesCount),
		m_indexCount(o.m_indexCount),
		m_vertexs( new V[m_vertexesCount*3] ),
		m_indices(new  I[m_indexCount])
	{
		unsigned int i;
		for (i=0; i<m_vertexesCount*3; i++)
			m_vertexs[i] = o.m_vertexs[i];
		for (i=0; i< m_indexCount; i++)
			m_indices[i] = o.m_indices[i];
	}
	// 比較
	bool operator==(const MeshBuffer<V,I>& o)
	{
		bool result = true;
		result  &= m_vertexesCount == o.m_vertexesCount;
		result  &= m_indexCount == o.m_indexCount;

		if (!result) return false;
		unsigned int i;
		for (i=0; i< m_indexCount; i++)
			result &= m_indices[i] == o.m_indices[i];
		if (!result) return false;
		for (i=0; i<m_vertexesCount*3; i++)
			result &= m_vertexs[i] == o.m_vertexs[i];
		return result;
	}
	//代入
	MeshBuffer<V,I>& operator=(const MeshBuffer<V,I>& rh)
	{
		m_vertexesCount = rh.m_vertexesCount;
		m_indexCount = rh.m_indexCount;
		for (unsigned int i=0; i< m_vertexesCount; i++)
			m_vertexs[i] = rh.m_vertexs[i];
		for (unsigned int i=0; i<m_indexCount; i++)
			m_indices = rh.m_indices[i];

		return *this;
	}
	//右辺値代入
	MeshBuffer<V,I>& operator=(MeshBuffer<V,I>&& rh)
	{
		m_vertexesCount = rh.m_vertexesCount;
		m_indexCount = rh.m_indexCount;
		m_vertexs.swap(rh.m_vertexs);
		m_indices.swap(rh.m_indices);

		return *this;
	}

	template<typename T>
		void getVertex(T& t, unsigned int index)
		{
			unsigned int i = index*3;
			assert(index < m_vertexesCount);
			t(m_vertexs[i], m_vertexs[i+1], m_vertexs[i+2]);
		}

	template<typename T,typename M>
		void getTriangle(M& m, unsigned int index)
		{
			T v[3];
			unsigned int verIndex;
			unsigned int maxVertexCount = index*3;
			for(unsigned int k=0; k <3; k++)
			{
				verIndex = m_indices[index+k];
				getVertex(v[k], verIndex);
			}
			m( v[0],v[1],v[2] );
		}

	//! btTriangleMeshを構成
	void createBtMesh(btTriangleMesh& mesh, bool deps=false) const
	{
		btVector3 v[3];
		unsigned int index;
		unsigned int maxVertexCount = m_vertexesCount*3;
		for(unsigned int i=0; i < m_indexCount; i+=3)
		{
			for(unsigned int k=0; k <3; k++)
			{
				index = m_indices[i+k] * 3;
				if (index < maxVertexCount)
					v[k].setValue( m_vertexs[index], m_vertexs[index+1], m_vertexs[index+2]);
			}
			mesh.addTriangle( v[0],v[1],v[2], deps);
		}
	}

	//! TriangleIndexより このオブジェクト自身を消すと死ぬので注意
	void createBtMesh(btTriangleIndexVertexArray& mesh, bool deps=false) const
	{
		btIndexedMesh m;
		m.m_numTriangles = m_indexCount/3;
		m.m_triangleIndexBase = (const unsigned char*)(&m_indices[0]);
		m.m_indexType = PHY_INTEGER;
		m.m_triangleIndexStride = 3*sizeof( I );

		m.m_numVertices = m_vertexesCount*3;
		m.m_vertexBase = (const unsigned char*)(&m_vertexs[0]);
		m.m_vertexStride = 3*sizeof( V );

		mesh.addIndexedMesh(m);
	}

	unsigned int getTriangleCount() const
	{
		return m_indexCount / 3;
	}

	// trasform by matrix4x4
	void transform(V const* m)
	{
		V x,y,z; //buf
		auto& v = m_vertexs;
		std::size_t i;
		for (std::size_t h=0; h < m_vertexesCount; ++h)
		{
			i = h*3;
			x = m[0]*v[i] + m[4]*v[i+1] + m[8 ]*v[i+2] + m[12];
			y = m[1]*v[i] + m[5]*v[i+1] + m[9 ]*v[i+2] + m[13];
			z = m[2]*v[i] + m[6]*v[i+1] + m[10]*v[i+2] + m[14];

			v[ i ] = x;
			v[i+1] = y;
			v[i+2] = z;
		}
	}
private:
	template<typename T>
		void irrConvert(const void* vertexs)
		{
			for (unsigned int i=0; i< m_vertexesCount; i++)
			{
				const T& v =  static_cast<const T*>(vertexs)[i];
				m_vertexs[i*3] = static_cast<V>(v.Pos.X);
				m_vertexs[i*3+1] = static_cast<V>(v.Pos.Y);
				m_vertexs[i*3+2] = static_cast<V>(v.Pos.Z);
			}
		}
};


namespace decomp
{
struct DecompDescriptBuilder
{
	//ConvexDecomposition::DecompDesc& desc_;
	ConvexDecomposition::ConvexDecompInterface& decomp_;
	unsigned int depth_;
	float cpercent_;
	float ppercent_;
	unsigned int maxVertices_;
	float skinWidth_;

	DecompDescriptBuilder(ConvexDecomposition::ConvexDecompInterface& d) :
		decomp_(d),
		depth_(5),
		cpercent_(5.f),
		ppercent_(15.f),
		maxVertices_(16),
		skinWidth_(0.f)
	{
	}
	bool process(const MeshBuffer<float,unsigned int>& buf)
	{
		ConvexDecomposition::DecompDesc desc_;

		desc_.mVcount      = buf.m_vertexesCount;
		desc_.mVertices    = buf.m_vertexs.get();
		desc_.mTcount      = buf.getTriangleCount();
		desc_.mIndices     = buf.m_indices.get();
		desc_.mDepth       = depth_;
		desc_.mCpercent    = cpercent_;
		desc_.mPpercent    = ppercent_;
		desc_.mMaxVertices = maxVertices_;
		desc_.mSkinWidth   = skinWidth_;

		desc_.mCallback = &decomp_;

		ConvexBuilder cb(desc_.mCallback);
		cb.process(desc_);
		return true;
	}
};
}
}
}

#endif
