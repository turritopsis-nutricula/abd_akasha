/*
 * btProfileDump.hpp
 *
 *  Created on: 2012/05/01
 *      Author: ely
 */

#ifndef BTPROFILEDUMP_HPP_
#define BTPROFILEDUMP_HPP_

#ifndef BT_NO_PROFILE

#include <LinearMath/btQuickprof.h>

namespace btHelper
{
namespace detail
{
template<typename F>
	void
	btProfileDumpRecursive(const F& func,
			CProfileIterator* profileIterator, int spacing)
{
	profileIterator->First();
	if (profileIterator->Is_Done())
		return;

	float
		accumulated_time = 0,
							  parent_time =
								  profileIterator->Is_Root() ?
								  CProfileManager::Get_Time_Since_Reset() :
								  profileIterator->Get_Current_Parent_Total_Time();
	int i;
	int frames_since_reset =
		CProfileManager::Get_Frame_Count_Since_Reset();


	func.parentInfo(profileIterator->Get_Current_Parent_Name(), parent_time);
	/*
		printf("Profiling: %s (total running time: %.3f ms) ---\n",
		profileIterator->Get_Current_Parent_Name(),
		parent_time);*/
	float totalTime = 0.f;

	int numChildren = 0;

	for (i = 0; !profileIterator->Is_Done();
			i++, profileIterator->Next()) {
		numChildren++;
		float current_total_time =
			profileIterator->Get_Current_Total_Time();
		accumulated_time += current_total_time;
		float fraction =
			parent_time > SIMD_EPSILON ?
			(current_total_time / parent_time) * 100 :
			0.f;
		func.childInfo(i, profileIterator->Get_Current_Name(), fraction,
				(current_total_time / (double) frames_since_reset),
				profileIterator->Get_Current_Total_Calls());
		/*
			printf("%d -- %s (%.2f %%) :: %.3f ms / frame (%d calls)\n",
			i, profileIterator->Get_Current_Name(), fraction,
			(current_total_time / (double) frames_since_reset),
			profileIterator->Get_Current_Total_Calls());*/
		totalTime += current_total_time;
		//recurse into children
	}

	if (parent_time < accumulated_time) {
		func.wrong();
		//printf("what's wrong\n");
	}

	func.unaccounted(
			parent_time > SIMD_EPSILON ?
			((parent_time - accumulated_time) / parent_time)* 100 : 0.f,
			parent_time - accumulated_time
			);
	/*printf(
	  "%s (%.3f %%) :: %.3f ms\n",
	  "Unaccounted:",
	  parent_time > SIMD_EPSILON ?
	  ((parent_time - accumulated_time) / parent_time)
	 * 100 : 0.f,
	 parent_time - accumulated_time);*/

	for (i = 0; i < numChildren; i++) {
		profileIterator->Enter_Child(i);
		btProfileDumpRecursive(func, profileIterator, spacing + 3);
		profileIterator->Enter_Parent();
	}
}
}  //namespace detail


//btProfileを全てDumpする関数
/**
 * プロフィールの始まり
 * func.parentInfo(const char* parentName, float parentTime) const;
 *
 * 同レベルプロフィールの列挙 :
 * func.childInfo(int number, const char* name, float fractionByParentTime,double averageTime,int callCount) const;
 *
 * プロフィール構造にエラー
 * func.wrong() const;
 *
 * プロフィール構造外の経過情報
 * func.unaccounted(float unaccountedTimeRate, float unaccountedTime) const;
 */
template<typename F>
	void
btProfileDump(const F& func)
{
	CProfileIterator* profileIterator = 0;
	profileIterator = CProfileManager::Get_Iterator();

	detail::btProfileDumpRecursive(func, profileIterator,0);

	CProfileManager::Release_Iterator(profileIterator);
}

}
#else

#endif //BT_NO_PROFILE
#endif /* BTPROFILEDUMP_HPP_ */
