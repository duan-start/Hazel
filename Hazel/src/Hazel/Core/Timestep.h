#pragma once

namespace Hazel {
	//用来表示的帧时间的变量
	class Timestep {
	public:
		Timestep(float time = 0.f) :
			m_Time(time) {

		}
		float GetSeconds()const { return m_Time; }
		float GetMillSeconds()const { return m_Time * 1000.f; }

		operator float() { return m_Time; }
	private:
		float m_Time ;
	};
}