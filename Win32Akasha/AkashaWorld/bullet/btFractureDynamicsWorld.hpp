/*
 * btFractureDynamicsWorld.hpp
 *
 *  Created on: 2012/09/09
 *      Author: ely
 */

#ifndef BTFRACTUREDYNAMICSWORLD_HPP_
#define BTFRACTUREDYNAMICSWORLD_HPP_

#define USE_TEMPLATE_DELIV 1

#include "btFractureBody.hpp"
#include "btFractureWelding.hpp"
#include "ConstraintHelper.hpp"

#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"


#include "BulletCollision/CollisionDispatch/btUnionFind.h"

#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>

#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/tuple/tuple.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>


#if(USE_TEMPLATE_DELIV)
template<typename BaseType, typename Info>
#endif
class btFractureDynamicsWorld :
#if (USE_TEMPLATE_DELIV)
	public BaseType
#else
	public btDiscreteDynamicsWorld
#endif
{
#if( !USE_TEMPLATE_DELIV)
	typedef btDiscreteDynamicsWorld BaseType;
	typedef btFractureDynamicsWorld ThisType;
#else
	typedef btFractureDynamicsWorld<BaseType, Info> ThisType;
#endif
	struct AddInfo
	{
		short int group_;
		short int mask_;
	};

	btAlignedObjectArray<btFractureBody*> m_suspendBodies;

	boost::unordered_map<btFractureBody*, AddInfo> m_suspendInfo;
	//typedef boost::unordered_multimap<btFractureBody*, boost::shared_ptr<btTypedConstraint>> CheinMapType;
	typedef boost::bimaps::bimap<
		boost::bimaps::unordered_multiset_of<btFractureBody*>,
		boost::bimaps::unordered_multiset_of<btTypedConstraint*>,
		//boost::bimaps::with_info<boost::shared_ptr<btTypedConstraint>>
		boost::bimaps::with_info<boost::weak_ptr<btTypedConstraint>>
			> CheinMapType;
	CheinMapType m_cheinConstraintMap;

	btAlignedObjectArray<btFractureBody*> m_rootBodies;
	std::vector<btFractureBody*> m_splitQueue;

	boost::unordered_map<btTypedConstraint*, boost::function<boost::shared_ptr<btTypedConstraint> (btTypedConstraint*, btTypedConstraint*)>> m_changeHandlers;
	//boost::unordered_map<boost::weak_ptr<btTypedConstraint>, boost::function<boost::shared_ptr<btTypedConstraint> (btTypedConstraint*, btTypedConstraint*)>> m_changeHandlers;
public:
	typedef boost::unordered_map<btTypedConstraint*, boost::shared_ptr<btTypedConstraint>> ConstraintMapType;


private:
	ConstraintMapType m_changeConstraintList;


public:
	struct ConstraintDeletor
	{
		ThisType* world_;
		void
			operator()(btTypedConstraint* t) const
			{
#ifndef NDEBUG
				std::cout << "ConstraintDelete ByDeleter." << std::endl;
#endif
				world_->removeConstraint(t);
			}
	};
	struct DefaultHandler
	{
		ThisType* world_;
		ConstraintMapType& m_;

		boost::shared_ptr<btTypedConstraint>
			operator()(btTypedConstraint*  oldConstraint, btTypedConstraint* newConstraint) const
			{
				boost::shared_ptr<btTypedConstraint> n(newConstraint,ConstraintDeletor{world_});
				m_[oldConstraint] = n;
				return n;
			}
	};
public:

	template<typename T>
		btFractureDynamicsWorld(T const& t) : BaseType(t){ }

	btFractureDynamicsWorld(btDispatcher* dispatcher,
			btBroadphaseInterface* pairCache,
			btConstraintSolver* constraintSolver,
			btCollisionConfiguration* collisionConfiguration) :
		BaseType(dispatcher, pairCache, constraintSolver,
				collisionConfiguration)
	{
	}
	/*~btFractureDynamicsWorld()
	  {

	  }*/

	//シミュレーションステップ オーバーライド
	int
		stepSimulation(	btScalar  	timeStep,int  	maxSubSteps = 1,btScalar  	fixedTimeStep = btScalar(1.)/btScalar(60.))
		{
			vairedConstraintCheinList(); //CheinMapを整理

			resetConnectionInfo(); //コネクション情報を初期化
			forceSplit(); //強制切断処理
			return BaseType::stepSimulation(timeStep, maxSubSteps, fixedTimeStep);
		}
	/// 拘束解決 オーバーライド
	void
		solveConstraints(btContactSolverInfo& solverInfo)
		{
			// todo: after fracture we should run the solver again for better realism
			// for example
			//	save all velocities and if one or more objects fracture:
			//	1) revert all velocties
			//	2) apply impulses for the fracture bodies at the contact locations
			//	3)and run the constaint solver again

			BaseType::solveConstraints(solverInfo);
			fractureCallback();
		}
	void
		synchronizeMotionStates()
		{
			BaseType::synchronizeMotionStates();

			synchronizeRootBodyMotionStates();

		}
	void
		synchronizeRootBodyMotionStates()
		{
			//rootBodysはここで上書き
			for (int i=0; i<m_rootBodies.size(); i++)
			{
				btFractureBody* body = m_rootBodies.at(i);
				assert(body->isWrap());
				btTransform const& t = static_cast<btCompoundShape*>(body->getCollisionShape())->getChildTransform(0);

				btTransform interpolatedTransform;
				btTransformUtil::integrateTransform(
						body->getInterpolationWorldTransform()*t,
						body->getInterpolationLinearVelocity(),
						body->getInterpolationAngularVelocity(),
						BaseType::m_localTime
						* body->getHitFraction(),
						interpolatedTransform);
				//body->getMineMotionState()->setWorldTransform(interpolatedTransform);

				body->getMotionState()->setWorldTransform(
						interpolatedTransform);

				//子のステータスを擬似更新
				body->updateChildBody();
			}
			//suspendBodyはここで追従更新
			for (int index = 0; index < m_suspendBodies.size(); index++) {
				btFractureBody* body = m_suspendBodies.at(index);
				BaseType::synchronizeSingleMotionState(body);
			}
		}

	void
		removeRigidBody(btRigidBody* b)
		{
			if (btFractureBody* f = btFractureBody::upcast(b))
			{
				m_rootBodies.remove(f);
				m_suspendBodies.remove(f);
				//TODO : FractureBody側の切断処理が必要?
			}
			BaseType::removeRigidBody(b);
		}

	void
		addConstraint(boost::shared_ptr<btTypedConstraint> constraint,bool disableCollisionsBetweenLinkedBodies)
		{
			addConstraint(constraint, disableCollisionsBetweenLinkedBodies, DefaultHandler{this, m_changeConstraintList});
		}
	template<typename Handler>
		void
		addConstraint(boost::shared_ptr<btTypedConstraint> constraint,bool disableCollisionsBetweenLinkedBodies, Handler handler)
		{
			typedef boost::shared_ptr<btTypedConstraint> constraintPtrType;
			constraintPtrType addConst = constraint;
			btRigidBody& body0 = constraint->getRigidBodyA();
			btRigidBody& body1 = constraint->getRigidBodyB();
			btFractureBody* casted0 = btFractureBody::upcast(&body0);
			btFractureBody* casted1 = btFractureBody::upcast(&body1);


			if (casted0 || casted1 ) //どちらかがbtFractureBody
			{
				bool flg0 =  casted0 && casted0->isUnion();
				bool flg1 =  casted1 && casted1->isUnion();

				if (flg0 || flg1) // どちらかが複合Body
				{
					btFractureBody* root0 = casted0 ?  helper::FractureBodyHelper{casted0}.getRootBody() : 0;
					btFractureBody* root1 = casted1 ?  helper::FractureBodyHelper{casted1}.getRootBody() : 0;

					//辻褄合わせにチェイン
					if (casted0)
						cheinConstraint(casted0, constraint);
					if (casted1)
						cheinConstraint(casted1, constraint);

					btTypedConstraint* n = helper::changeConstraintBody(constraint.get(),root0 ?  *root0 : body0,root1 ? *root1 : body1);
					auto p = handler(constraint.get(), n);

					addConst.swap(p);


					if (addConst)
					{
						helper::ConstraintPivotShift(*addConst,
								casted0 ? helper::FractureBodyHelper{casted0}.getWorldTransform() : body0.getWorldTransform(),
								casted1 ? helper::FractureBodyHelper{casted1}.getWorldTransform() : body1.getWorldTransform(),
								//body0.getWorldTransform(),
								//body1.getWorldTransform(),
								(root0 ?  *root0 : body0).getWorldTransform(),
								(root1 ? *root1 : body1).getWorldTransform());
					}
					//addChangeConstraint(constraint, addConst);
				}

			}



			if (casted0 && addConst) // btFractureBody かつ 有効なコンストライン
				cheinConstraint(casted0, addConst);
			if (casted1 && addConst)
				cheinConstraint(casted1, addConst);

			if (addConst)
			{
				if (casted0 || casted1) //どちらがbtFractureBody
					m_changeHandlers[addConst.get()] = handler;

				internalAddConstraint(addConst, disableCollisionsBetweenLinkedBodies);
			}
		}
	void
		removeConstraint(boost::shared_ptr<btTypedConstraint> constraint)
		{
			removeConstraint(constraint.get());
		}

	void
		removeConstraint(btTypedConstraint* constraint)
		{
			btFractureBody* casted0 = btFractureBody::upcast(&constraint->getRigidBodyA());
			btFractureBody* casted1 = btFractureBody::upcast(&constraint->getRigidBodyB());

			if (casted0)
				uncheinConstraint(casted0, constraint);

			if (casted1)
				uncheinConstraint(casted1, constraint);

			if (m_changeHandlers.count(constraint)==1)
				m_changeHandlers.erase(constraint);

			internalRemoveConstraint(constraint);
		}

	btFractureWelding
		addWeld(btFractureBody& b0, btFractureBody& b1)
		{
			//TODO : addRigidBodyを前提とする?
			btFractureBody* leafRoot = 0;
			if (b1.isUnion())
				leafRoot = helper::FractureBodyHelper{&b1}.getRootBody();
			else
				leafRoot = &b1;


			//以下Constraint処理//////////////////////////////////
			typedef boost::unordered_map<btTypedConstraint*, std::pair<btTransform, btTransform>> transMapType;
			transMapType transBufMap;
			btFractureBody* rootRoot = &b0;
			//typedef typename CheinMapType::value_type valueType;
			typedef typename CheinMapType::left_reference valueType;
			typedef typename CheinMapType::left_iterator iterType;
			typedef boost::shared_ptr<btTypedConstraint> constraintPtrType;
			//typedef btTypedConstraint* constraintPtrType;

			std::vector<constraintPtrType> addBuf, removeBuf;
			iterType conI, conEnd;
			//b1側 rootのRootに付け替え
			boost::for_each(helper::FractureBodyHelper{leafRoot}.getUnionBodiesList(),  [&](btFractureBody* body){
					addBuf.clear(); removeBuf.clear();
					boost::tie(conI, conEnd) = m_cheinConstraintMap.left.equal_range(body);

					for (;conI!=conEnd; conI++)
					{
					if (constraintPtrType oldC = conI->info.lock())
					{
					assert(m_changeHandlers.count(oldC.get())==1);

					constraintPtrType newC(
							m_changeHandlers[oldC.get()](
								oldC.get(),
								helper::changeConstraintBody(
									oldC.get(),
									&(oldC->getRigidBodyA())==leafRoot ? *rootRoot : oldC->getRigidBodyA(),
									&(oldC->getRigidBodyB())==leafRoot ? *rootRoot : oldC->getRigidBodyB()
									)));
					//this->addChangeConstraint(oldC, newC);

					removeBuf.push_back(oldC);
					if (newC)
					{
						transBufMap[ newC.get() ]={oldC->getRigidBodyA().getWorldTransform(), oldC->getRigidBodyB().getWorldTransform() };
						addBuf.push_back(newC);
					}
					else
						m_changeHandlers.erase(oldC.get());

					}
					}
					boost::for_each(addBuf,[&](constraintPtrType c) {
							this->internalAddConstraint(c,true);
							this->cheinConstraint(body,c);
							});
					boost::for_each(removeBuf,[&](constraintPtrType c){
							this->internalRemoveConstraint(c);
							this->uncheinConstraint(body,c);
							});
			});



			//b0側 座標変換のみ
			boost::for_each(helper::FractureBodyHelper{rootRoot}.getUnionBodiesList(),
					[&](btFractureBody* body){
					boost::tie(conI, conEnd) = m_cheinConstraintMap.left.equal_range(body);
					for (; conI!=conEnd; ++conI)
					{
					if (conI->info.lock())
					{
					btTypedConstraint* c = conI->second;
					transBufMap[ c ] = {c->getRigidBodyA().getWorldTransform(), c->getRigidBodyB().getWorldTransform() };
					}}
					});

			//Body再列//////////////////////////////////
			leafRoot->resetRootBody(&b0);
			b0.resetConnections_();

			m_rootBodies.remove(&b0);
			m_rootBodies.remove(leafRoot);
			if (b0.isWrap())
				m_rootBodies.push_back(&b0);

			suspendBody(leafRoot);

			//Constraint更新//////////////////////////////
			boost::for_each(transBufMap, [](typename transMapType::value_type const& v){
					btTypedConstraint* c = v.first;
					helper::ConstraintPivotShift(*c, v.second.first, v.second.second, c->getRigidBodyA().getWorldTransform(), c->getRigidBodyB().getWorldTransform());
					});

			//Transform更新//////////////////////////////
			b0.updateChildBody();
			boost::for_each(helper::FractureBodyHelper{&b0}.getUnionBodiesList(),[](btFractureBody* f){
					f->setInterpolationWorldTransform(f->getInterpolationWorldTransform());
					//f->setInterpolationAngularVelocity(f->getAngularVelocity());
					f->activate(true);
					});
			//b0.setInterpolationWorldTransform(b0.getWorldTransform());


			return btFractureWelding(b0, b1);
		}

	//任意のFractureBodyを切り離す
	void
		splitBody(btFractureBody& f)
		{
			m_splitQueue.push_back(&f);
		}



	//チェンジコンストラインの処理
	template<typename F>
		void
		changeConstraintFunction(const F& f)
		{
			f(m_changeConstraintList);
		}

private:

	// ConnectionInfoを全て初期化
	void
		resetConnectionInfo() const
		{

			for (int j=0; j<m_rootBodies.size(); j++)
			{
				btFractureBody* f = m_rootBodies.at(j);
				for(int i=0; i<f->getChildBodyCount(); i++)
					f->getBodyPtrByShapeIndex(i)->resetConnectionInfo();
			}
		}

	// splitQueueに入ってるbodyを強制切断
	void
		forceSplit()
		{
			if (!m_splitQueue.size()) return;
			typedef boost::unordered_set<btFractureBody*> MapType;
			MapType map;
			boost::for_each(m_splitQueue, [&map](btFractureBody* b){
					btFractureBody* r = helper::FractureBodyHelper{b}.getRootBody();
					if (r!=b)
					{
					map.insert(r);
					btFractureBody::connectionInfo& c = b->getConnectionInfo();
					c.bufTotalImpulse_ += c.breakImpulse_;
					}
					});
			m_splitQueue.clear();

			boost::for_each(map, [&](typename MapType::value_type v){
					this->breakDisconnectedParts(v);
					});

			{ //m_rotBodyesの掃除

				int last = m_rootBodies.size();
				for (int i = 0; i < last; i++) {
					if (!m_rootBodies.at(i)->isWrap()) {

						resetBody(m_rootBodies.at(i));
						m_rootBodies.swap(i, last - 1);
						i--;
						last--;
					}
				}
				m_rootBodies.resize(last);
			}
		}

private:
	struct btFracturePair
	{
		bool bufIsNeedBreak_;
		btFractureBody* body_;
		btAlignedObjectArray<btPersistentManifold*> contactManifolds_;
	};

	struct FracturePairMap
	{
		btAlignedObjectArray<btFracturePair> map_;

		btFracturePair&
			add(btFractureBody* b, btPersistentManifold* m=0)
			{
				int pi = -1;

				for (int p = 0; p < map_.size(); p++) {
					if (map_[p].body_ == b) {
						pi = p;
						break;
					}
				}

				if (pi < 0) {
					btFracturePair p{false};
					p.body_ = b;
					if (m)
						p.contactManifolds_.push_back(m);
					map_.push_back(p);
					return map_.at(map_.size()-1);
				}
				else {
					if (m)
					{
						btAssert(
								map_[pi].contactManifolds_.findLinearSearch(
									m)
								== map_[pi].contactManifolds_.size());

						map_[pi].contactManifolds_.push_back(m);
					}
					return map_.at(pi);
				}

			}
		int
			size() const
			{
				return map_.size();
			}
		btFracturePair&
			at(int n)
			{
				return map_.at(n);
			}


	};
private:

	///normally those callbacks are called internally by the 'solveConstraints'
	void
		fractureCallback()
		{

			auto& bodies = m_rootBodies;


			FracturePairMap sFracturePairs;

			int numManifolds = BaseType::getDispatcher()->getNumManifolds();
			//sFracturePairs.clear();

			for (int i = 0; i < numManifolds; i++) {
				btPersistentManifold* manifold =
					BaseType::getDispatcher()->getManifoldByIndexInternal(i);

				if (!manifold->getNumContacts())
					continue;

				//@todo: add better logic to decide what parts to fracture
				//For example use the idea from the SIGGRAPH talk about the fracture in the movie 2012:
				//
				//Breaking thresholds can be stored as connectivity information between child shapes in the fracture object
				//
				//You can calculate some "impact value" by simulating all the individual child shapes
				//as rigid bodies, without constraints, running it in a separate simulation world
				//(or by running the constraint solver without actually modifying the dynamics world)
				//Then measure some "impact value" using the offset and applied impulse for each child shape
				//weaken the connections based on this "impact value" and only break
				//if this impact value exceeds the breaking threshold.
				//you can propagate the weakening and breaking of connections using the connectivity information
				//std::cout << "first if." << std::endl;

				const int f0 = bodies.findLinearSearch(
						(btFractureBody*) manifold->getBody0());
				const int f1 = bodies.findLinearSearch(
						(btFractureBody*) manifold->getBody1());
				{  //両方ともFractureBodyではない
					if (f0 == bodies.size() && f1 == bodies.size())
						continue;
				}
				{  //接触点の大まかな応力
					btScalar totalImpact = 0.f;
					for (int p = 0; p < manifold->getNumContacts(); p++)
						totalImpact +=
							manifold->getContactPoint(p).m_appliedImpulse;

					static float maxImpact = 0;
					if (totalImpact > maxImpact)
						maxImpact = totalImpact;

					//some threshold otherwise resting contact would break objects after a while
					if (totalImpact < 100.f)
						continue;

					//		printf("strong impact\n");
				}

				//f0について情報保存
				if (f0 < bodies.size() )
					sFracturePairs.add(bodies[f0], manifold);
				//f1について情報保存
				if (f1 < bodies.size())
					sFracturePairs.add(bodies[f1], manifold);


			}  // manifold loop

			//rootに接続されてるコンストライン
			{
				typedef typename CheinMapType::right_iterator iterType;
				iterType ic, end;
				for (int i=0; i<bodies.size(); i++)
				{
					btFractureBody* b = bodies.at(i);
					for (int j=0; j<b->getNumConstraintRefs(); j++)
					{
						btTypedConstraint* c = b->getConstraintRef(j);
						if ( (!c->needsFeedback()) ? c->getAppliedImpulse() < 1.f : true) continue;

						boost::tie(ic,end) = m_cheinConstraintMap.right.equal_range(c);
						int count = std::distance(ic,end);
						assert(count>0 && count < 3);
						for (; ic!=end; ++ic)
						{
							//TODO : 場合によってはlockとらないとダメかも
							if (ic->first->needsFeedback() && (ic->first->getAppliedImpulse() > ic->second->getConnectionInfo().threadImpulse_))
							{
								ic->second->getConnectionInfo().bufTotalImpulse_ += ic->first->getAppliedImpulse() / btScalar(count);
								//sFracturePairs.add(ic->second).bufIsNeedBreak_=true;
								sFracturePairs.add(b).bufIsNeedBreak_=true;
							}}
					}}
			}

			if (!sFracturePairs.size())
				return;

			//		printf("fracturing\n");

			for (int i = 0; i < sFracturePairs.size(); i++) {
				//check impulse/displacement at impact
				//weaken/break connections (and propagate breaking)
				//compute connectivity of connected child shapes
				btFracturePair& fracturePair = sFracturePairs.at(i);
				btFractureBody* body = fracturePair.body_;
				btAlignedObjectArray<btPersistentManifold*>& manis =
					fracturePair.contactManifolds_;

				if (!(body->isWrap()))
					continue;

				btCompoundShape* oldCompound =
					static_cast<btCompoundShape*>(body->getCollisionShape());

				assert(oldCompound->getNumChildShapes() > 1);





				//@todo: propagate along the connection graph
				for (int j = 0; j < manis.size(); j++) {
					btPersistentManifold* manifold = manis[j];

					int pt_index;
					for (int k = 0; k < manifold->getNumContacts(); k++) {
						btManifoldPoint const& pt = manifold->getContactPoint(k);

						pt_index =
							(manifold->getBody0() == body) ?
							pt.m_index0 : pt.m_index1;
						//rootはスキップ
						for (int f=1; f < body->getChildBodyCount(); f++)
						{
							btFractureBody* childBody = body->getBodyPtrByShapeIndex(f);
							btFractureBody::connectionInfo& c = childBody->getConnectionInfo();
							if (f==pt_index && c.threadImpulse_<pt.m_appliedImpulse)
							{
								c.bufTotalImpulse_ += pt.m_appliedImpulse;
								fracturePair.bufIsNeedBreak_= true;
							}
						}

					}
				}  // for j

				if (fracturePair.bufIsNeedBreak_)
					breakDisconnectedParts(body);
			}  //sFractureLoop

			{

				int last = m_rootBodies.size();
				for (int i = 0; i < last; i++) {
					if (!m_rootBodies.at(i)->isWrap()) {

						resetBody(m_rootBodies.at(i));
						m_rootBodies.swap(i, last - 1);
						i--;
						last--;
					}
				}
				m_rootBodies.resize(last);
			}
		}




	////////////////
	void
		breakDisconnectedParts(btFractureBody* body)
		{

			assert(body->isWrap());

			btCompoundShape* compound =
				(btCompoundShape*) body->getCollisionShape();
			const int numChildren = compound->getNumChildShapes();

			assert(numChildren>1);

			//compute connectivity
			btUnionFind unionFind;

			btAlignedObjectArray<int> tags;
			tags.resize(numChildren);
			int i, index = 0;
			for (i = 0; i < numChildren; i++) {
#ifdef STATIC_SIMULATION_ISLAND_OPTIMIZATION
				tags[i] = index++;
#else
				tags[i] = i;
				index = i + 1;
#endif
			}

			unionFind.reset(index);


			boost::for_each(helper::FractureBodyHelper{body}.getUnionBodiesList(),[&unionFind, &tags](btFractureBody* b){
					btFractureBody::connectionInfo& c = b->getConnectionInfo();
					if (c.rootBody_!=0 && c.breakImpulse_>c.bufTotalImpulse_)
					{
					unionFind.unite(
							tags[c.childIndex_] ,
							tags[c.connectionBody_->getConnectionInfo().childIndex_]
							);
					}
					});


			int numElem = unionFind.getNumElements();

			index = 0;
			for (int ai = 0; ai < numChildren; ai++) {
				tags[ai] = unionFind.find(index);
				//Set the correct object offset in Collision Object Array
#if STATIC_SIMULATION_ISLAND_OPTIMIZATION
				unionFind.getElement(index).m_sz = ai;
#endif //STATIC_SIMULATION_ISLAND_OPTIMIZATION
				index++;
			}


			unionFind.sortIslands();



			int endIslandIndex = 1;
			int startIslandIndex;

			int numIslands = 0;

			/*btAlignedObjectArray < btAlignedObjectArray
			  < boost::uuids::uuid >> uuid_tree;*/
			typedef btAlignedObjectArray<btFractureBody*> list_type;
			btAlignedObjectArray<list_type> body_island;
			for (startIslandIndex = 0; startIslandIndex < numElem;
					startIslandIndex = endIslandIndex) {
				int islandId = unionFind.getElement(startIslandIndex).m_id;
				for (endIslandIndex = startIslandIndex + 1;
						(endIslandIndex < numElem)
						&& (unionFind.getElement(endIslandIndex).m_id
							== islandId); endIslandIndex++) {
				}
				//startIsLandIndexとendIsLandIndexまでIsland番号がならんでる
				int numShapes = 0;


				list_type list;
				int idx;
				for (idx = startIslandIndex; idx < endIslandIndex; idx++) {
					int i = unionFind.getElement(idx).m_sz;  //m_szにChildShapeIndexが入ってるらしい

					list.push_back(body->getBodyPtrByShapeIndex(i));
					if (unionFind.getElement(idx).m_id==unionFind.getElement(idx).m_sz)
					{
						list.swap(0, list.size()-1);
					}

					numShapes++;
				}

				if (numShapes > 0) {

					body_island.push_back(list);
					numIslands++;
				}
			}

			//最終的に分割されない
			if (body_island.size()<2) return;

			//生成されたハッシュツリーからオブジェクトを再構成
			const btTransform bufFracWorldTrans = body->getWorldTransform();
			boost::unordered_map<btFractureBody*, btTransform> transMap;
			transMap[body] = static_cast<btCompoundShape*>(body->getCollisionShape())->getChildTransform(0);
			for (int j=0; j<body_island.size(); j++)
			{
				int index = body->getChildShapeIndexByBodyPtr(body_island[j][0]);
				transMap[body_island[j][0]] = static_cast<btCompoundShape*>(body->getCollisionShape())->getChildTransform(index);

			}


			//Bodyを切り離し
			int rootIslandIndex = -1;
			for (int j = 0; j < body_island.size(); j++) {

				list_type const& list = body_island[j];

				bool useMine = false;
				for (int i = 0; i < list.size() && rootIslandIndex == -1; i++)
				{
					useMine = list[i] == body || useMine;
				}
				if (useMine) {

					rootIslandIndex = j;
					continue; //自分のツリーなのでスルー
				}




				btFractureBody* newBody = list[0];  //0番をRootとする
				assert(transMap.count(newBody)==1);
				newBody->setWorldTransform(bufFracWorldTrans * transMap.at(newBody));

				{//コンストライン処理
					typedef boost::unordered_map<btTypedConstraint*, std::pair<btTransform,btTransform> > transMapType;
					transMapType transBufMap;
					typedef typename CheinMapType::left_iterator iterType;
					typedef boost::shared_ptr<btTypedConstraint> constPtrType;
					iterType iter, end;
					constPtrType newConstraint;
					std::vector<constPtrType> removeBuf, addBuf;
					for (int i = 0; i < list.size(); i++) {

						btFractureBody* child = list[i];

						transBufMap.clear();
						removeBuf.clear();
						addBuf.clear();


						boost::tie(iter,end) = m_cheinConstraintMap.left.equal_range(child);

						for (; iter!=end; iter++) {
							btTypedConstraint* oldConstPtr = iter->second;
							assert(m_changeHandlers.count(oldConstPtr)==1);
							auto const& handle = m_changeHandlers.at(oldConstPtr);
							if(boost::shared_ptr<btTypedConstraint> constraintPtr = iter->info.lock())
							{
								if (&(constraintPtr->getRigidBodyA()) == body)
									newConstraint = handle(oldConstPtr,
											helper::changeConstraintBody(
												constraintPtr.get(), *newBody,
												constraintPtr->getRigidBodyB()));
								else if (&(constraintPtr->getRigidBodyB()) == body)
									newConstraint = handle(oldConstPtr,
											helper::changeConstraintBody(
												constraintPtr.get(), constraintPtr->getRigidBodyA(),
												*newBody));

								//assert(newConstraint);
								removeBuf.push_back(constraintPtr);
								if (newConstraint)
								{
									transBufMap[newConstraint.get()] = {
										constraintPtr->getRigidBodyA().getWorldTransform(),
										constraintPtr->getRigidBodyB().getWorldTransform()
									};
									addBuf.push_back(newConstraint);
								}
								else
									m_changeHandlers.erase(oldConstPtr);
								//addChangeConstraint(constraintPtr, newConstraint);
							}
						}

						//constaint付け替え
						boost::for_each(removeBuf,[&](constPtrType& c){
								this->internalRemoveConstraint(c);
								this->uncheinConstraint(child ,c);
								});
						boost::for_each(addBuf,[&](constPtrType& c){
								this->internalAddConstraint(c,true);
								this->cheinConstraint(child, c);
								});
						boost::for_each(transBufMap,[](typename transMapType::value_type& v){
								btTypedConstraint* c = v.first;
								helper::ConstraintPivotShift(*c,
										v.second.first,
										v.second.second,
										c->getRigidBodyA().getWorldTransform(),
										c->getRigidBodyB().getWorldTransform());
								});
					}


				}
				newBody->fractBody(); //切り離し

				newBody->setLinearVelocity(body->getLinearVelocity());
				newBody->setAngularVelocity(body->getAngularVelocity());
				weakUpBody(newBody);
				if (newBody->isWrap())
				{
					assert(m_rootBodies.findLinearSearch(newBody)==m_rootBodies.size());
					addRootBodies(newBody);
				}

			}
			assert(body->getChildShapeIndexByBodyPtr(body)==0);
			assert(transMap.count(body)==1);


			if (body->isWrap())
				body->setWorldTransform(bufFracWorldTrans*transMap[body] * static_cast<btCompoundShape*>(body->getCollisionShape())->getChildTransform(0).inverse());
			else
				body->setWorldTransform(bufFracWorldTrans*transMap[body]);


			//残ったツリーのコンストライン処理
			{

				assert(rootIslandIndex>=0);

				typedef typename CheinMapType::left_iterator iterType;
				iterType iter, end;

				assert(transMap.count(body)==1);
				const btTransform oldTransform(transMap[body]);

				for (int i=0; i<body_island[rootIslandIndex].size();i++)
				{
					btFractureBody* f = body_island[rootIslandIndex][i];
					boost::tie(iter,end) = m_cheinConstraintMap.left.equal_range(f);
#ifndef NDEBUG
					std::cout << rootIslandIndex << ":" << i << "  " << f << " is " << std::distance(iter, end) << std::endl;
#endif
					for(; iter!=end; iter++)
					{

						if (!iter->info.expired())
						{
							btTypedConstraint* c = iter->second;
							helper::ConstraintPivotShift(*c,
									&(c->getRigidBodyA()) != f,
									bufFracWorldTrans,
									body->getWorldTransform()
									);
						}
					}
				}

			}



		}

	void
		addRootBodies(btFractureBody* f)
		{
			assert(	BaseType::m_collisionObjects.size()!=BaseType::m_collisionObjects.findLinearSearch(f));

			if (f->isWrap())
				m_rootBodies.push_back(f);
		}
	void
		weakUpBody(btFractureBody* b)
		{
			int i = m_suspendBodies.findLinearSearch(b);
			if (i != m_suspendBodies.size()) {
				assert(m_suspendInfo.count(b)==1);
				BaseType::addRigidBody(m_suspendBodies.at(i), m_suspendInfo[b].group_, m_suspendInfo[b].mask_);
				m_suspendBodies.remove(b);
				m_suspendInfo.erase(b);
			}
		}
	//
	void
		suspendBody(btFractureBody* b)
		{
			assert(m_suspendBodies.size()==m_suspendBodies.findLinearSearch(b));
			assert(BaseType::getCollisionObjectArray().findLinearSearch(b)!=BaseType::getCollisionObjectArray().size());

			m_suspendBodies.push_back(b);
			m_suspendInfo[b] =
				AddInfo{
					b->getBroadphaseProxy()->m_collisionFilterGroup,
					b->getBroadphaseProxy()->m_collisionFilterMask };
			BaseType::removeRigidBody(b);

		}
	//CollisionShapeが切り替わった際に呼び出し
	void
		resetBody(btFractureBody* b)
		{
			short int g = b->getBroadphaseProxy()->m_collisionFilterGroup;
			short int m = b->getBroadphaseProxy()->m_collisionFilterMask;
			BaseType::removeRigidBody(b);
			BaseType::addRigidBody(b, g, m);
		}


	void
		cheinConstraint(btFractureBody* b, boost::shared_ptr<btTypedConstraint> c)
		{
			// 		std::cout << "chain:" << b << ":" << c.get() << "::" << m_cheinConstraintMap.left.size() <<  std::endl;
			m_cheinConstraintMap.insert(typename CheinMapType::value_type{b, c.get(), boost::weak_ptr<btTypedConstraint>(c)});
		}
	void
		uncheinConstraint(btFractureBody* b, boost::shared_ptr<btTypedConstraint> c)
		{
			uncheinConstraint(b, c.get());
		}
	void
		uncheinConstraint(btFractureBody* b, btTypedConstraint* c)
		{
			typename CheinMapType::left_iterator i,end;
			boost::tie(i,end) = m_cheinConstraintMap.left.equal_range(b);

			// 		std::cout << "unchain:" << b << ":" << c << "::" << m_cheinConstraintMap.left.size();
			for(;i != end; i++)
			{
				if (i->second == c)
				{
					m_cheinConstraintMap.left.erase(i);
					// 				std::cout << "hit";
					break;
				}
			}
			// 		std::cout <<  std::endl;
		}
	void
		vairedConstraintCheinList()
		{
			std::vector<btTypedConstraint*> buf;
			typename CheinMapType::right_iterator i,end;
			end = m_cheinConstraintMap.right.end();
			for(i=m_cheinConstraintMap.right.begin(); i!=end; i++)
			{
				if (i->info.expired())
					buf.push_back(i->first);
			}

#ifndef NDEBUG
			{
				if (buf.size())
					std::cout << "CheinMap Erase by " << buf.size() << std::endl;
			}
#endif
			boost::for_each(buf,[&](btTypedConstraint* t){
					boost::tie(i,end) = m_cheinConstraintMap.right.equal_range(t);
					m_cheinConstraintMap.right.erase(i,end);
					});


		}


	void
		internalAddConstraint(boost::shared_ptr<btTypedConstraint> c, bool disableCollisionsBetweenLinkedBodies)
		{
			BaseType::addConstraint(c.get(), disableCollisionsBetweenLinkedBodies);
		}
	void
		internalRemoveConstraint(boost::shared_ptr<btTypedConstraint> c)
		{
			internalRemoveConstraint(c.get());
		}
	void
		internalRemoveConstraint(btTypedConstraint* c)
		{
			BaseType::removeConstraint(c);
		}
private:
};

#if(USE_TEMPLATE_DELIV)
#include <boost/mpl/arg.hpp>
namespace akasha{ namespace bullet{
typedef btFractureDynamicsWorld<boost::mpl::_1, boost::mpl::_2> FractureDynamics;
}}
#endif

#endif /* BTFRACTUREDYNAMICSWORLD_HPP_ */
