#include "Bazier.h"

#include <vector>
#include <map>
#include <cmath>

namespace _Bhyin {
	struct Vec3
	{
		Vec3() :_v{ 0.0f,0.0f,0.0f } {}

		Vec3(const Vec3& other)
		{
			_v[0] = other._v[0], _v[1] = other._v[1], _v[2] = other._v[2];
		}

		Vec3(double ix, double iy, double iz)
		{
			_v[0] = ix, _v[1] = iy, _v[2] = iz;
		}

		inline double* vector() { return _v; }

		// 实现基本的加法和数乘重载
		Vec3 operator+(const Vec3& other) const
		{
			return Vec3{ _v[0] + other._v[0],_v[1] + other._v[1],_v[2] + other._v[2] };
		}

		Vec3 operator*(const double m) const
		{
			return Vec3{ _v[0] * m, _v[1] * m,_v[2] * m };
		}
	private:
		double _v[3];
	};
}

namespace Bhyin
{

	// 利用i和k可以唯一确定一个顶点，可以用一个散列表存储
	std::map<std::pair<int, int>, _Bhyin::Vec3> CurvePoints;

	// 控制点
	std::vector<_Bhyin::Vec3> CurveHandles;

	// de Castejau算法
	_Bhyin::Vec3 de_Castejau(int k, int i, double t)
	{
		// 结果
		_Bhyin::Vec3 result;

		// 键
		auto p = std::pair<int, int>(k, i);
		// 是否已经计算过
		auto it = CurvePoints.find(p);
		// 如果没计算过
		if (it == CurvePoints.end())
		{
			//若k为0，直接返回对应控制点，否则递归
			// 另外，若k为0，就不往Points字典里存顶点了
			if (k == 0)result = CurveHandles[i];
			else
			{
				result = de_Castejau(k - 1, i, t) * (1.0 - t) + de_Castejau(k - 1, i + 1, t) * t;
				CurvePoints[p] = result;// 把计算过的值保存
			}

		}
		else result = CurvePoints[p]; // 若计算过，直接返回计算过的值
		/*printf("i: %d, k: %d, vec:(%f, %f, %f)\n", i, k, result.vector()[0], result.vector()[1], result.vector()[2]);*/
		//返回结果
		return result;
	}

	BazierCurve::BazierCurve() :exist(false), s_HandlesNum(0), s_SampleNum(200), s_FrameSize(-1), s_Handles(nullptr), s_DrawData(nullptr)
	{
	}

	BazierCurve::BazierCurve(double* handles, int N, int sample_num) : exist(false), s_HandlesNum(N), s_SampleNum(sample_num), s_FrameSize(-1), s_Handles(nullptr), s_DrawData(nullptr)
	{
		exist = s_HandlesNum > 0;
		// 初始化控制点
		for (int i = 0; i < s_HandlesNum; ++i)
			CurveHandles.push_back(_Bhyin::Vec3{ handles[3 * i], handles[3 * i + 1], handles[3 * i + 2] });

		// 计算数据
		if (s_HandlesNum != 0)RefreshCurve();
	}

	void BazierCurve::AppendHandle(double x, double y, double z)
	{
		// 添加控制点
		CurveHandles.push_back(_Bhyin::Vec3{ x,y,z });
		s_HandlesNum += 1;
		exist = true;

		RefreshCurve();
	}

	void BazierCurve::RefreshCurve()
	{
		s_FrameSize = -1; // 把每一帧计算的控制点总数调整到初始状态

		//此外还应当清除原有数据，不过这已经在GenerateCurve中调用ClearData实现了。

		// 重新计算数据
		GenerateCurve();
	}

	void BazierCurve::GenerateCurve()
	{
		// 初始化DrawData,每个顶点3个分量，每次采样需要(s_HandlesNum + 1) * s_HandlesNum / 2个顶点，共采样s_SampleNum次
		// 不过每一次计算，控制点的值都是不变的，而且已经保存在s_Handles里了，可以从DrawData中省去，
		// 这样一来可以减少 s_HandlesNum * s_SampleNum 个double的存储空间
		// 式子可以修改如下
		ClearDrawData();

		s_DrawData = new double[3 * GetFrameSize() * s_SampleNum + 200];

		// 一些需要频繁创建和销毁的变量，移动到循环之外
		int pointNum = 0, frameIdx = 0, pointIdx = 0, j = 0;
		double* temp;

		// 计算贝塞尔曲线生成过程产生的所有的点
		for (int i = 0; i < s_SampleNum; ++i)
		{
			CurvePoints.clear(); // 清空字典

			de_Castejau(s_HandlesNum - 1, 0, double(i) / s_SampleNum);
			//将字典中的值转移到DrawData上
			/*map默认有序，以键为基准升序排序，pair则是先比较第一个元素再比较第二个元素
			所以map中的Vec对象会以(0,0),(0,1),...,(1,0),(1,1),...,(2,0),(2,1),...的顺序排列
			将其转移到DrawData上，然后使用glDrawArray来绘制，比较容易开发
			*/

			pointNum = CurvePoints.size();
			frameIdx = i * pointNum * 3;
			for (auto it : CurvePoints)
			{
				pointIdx = j * 3;
				temp = it.second.vector();
				s_DrawData[frameIdx + pointIdx] = temp[0];
				s_DrawData[frameIdx + pointIdx + 1] = temp[1];
				s_DrawData[frameIdx + pointIdx + 2] = temp[2];
				++j;
			}

			j = 0;

			CurvePoints.clear();
		}
	}

	void BazierCurve::isHandleDragged(double x, double y) {
		double dx, dy;
		// 遍历所有控制点，判断是否有某个点位于x、y附近
		for (int i = 0; i < s_HandlesNum; ++i)
		{
			dx = x - CurveHandles[i].vector()[0];
			dy = y - CurveHandles[i].vector()[1];
			// 距离小于某值视为被点击
			if (sqrt(dx * dx + dy * dy) < 0.1f)
			{
				isDragging = true;
				dragIndex = i;
				break;
			}
		}
	}

	void BazierCurve::Drag(double x, double y)
	{
		CurveHandles[dragIndex] = _Bhyin::Vec3(x, y, 0.0);
	}

	int BazierCurve::GetFrameSize()
	{
		if (s_FrameSize == -1)s_FrameSize = (s_HandlesNum - 1) * s_HandlesNum / 2;
		return s_FrameSize;
	}

	double* BazierCurve::GetHandlesData()
	{
		if (s_Handles)return s_Handles;
		else
		{
			s_Handles = new double[s_HandlesNum * 3 + 9];
			for (int i = 0; i < CurveHandles.size(); ++i)
			{
				s_Handles[3 * i] = CurveHandles[i].vector()[0];
				s_Handles[3 * i + 1] = CurveHandles[i].vector()[1];
				s_Handles[3 * i + 2] = CurveHandles[i].vector()[2];
			}
			return s_Handles;
		}
	}


	BazierCurve::~BazierCurve()
	{
		ClearAllData();
	}

	void BazierCurve::ClearAllData()
	{
		s_HandlesNum = 0;
		s_SampleNum = 0;
		s_FrameSize = -1;
		exist = false;
		ClearDrawData();
		CurveHandles.clear();
		CurvePoints.clear();
	}

	void BazierCurve::ClearDrawData()
	{
		if (s_Handles)
		{
			delete[]s_Handles;
			s_Handles = nullptr;
		}

		if (s_DrawData)
		{
			delete[]s_DrawData;
			s_DrawData = nullptr;
		}
	}


	/**********************************以下是贝塞尔曲面************************************/
	/**********************************以下是贝塞尔曲面************************************/
	/**********************************以下是贝塞尔曲面************************************/
	// 保存所有曲面的控制点的数据
	std::vector<std::vector<_Bhyin::Vec3>> SurfaceHandles;

	// 保存计算过程中单根曲线所有顶点数据
	std::map<std::pair<int, int>, _Bhyin::Vec3> SingleCurvePoints;
	// 保存计算过程中单根曲线的控制点数据
	std::vector<_Bhyin::Vec3>  SingleCurveHandles;

	// 计算单根曲线的数据
	_Bhyin::Vec3 CalSingleCurveData(int k, int i, double t)
	{
		return k == 0 ? SingleCurveHandles[i] : CalSingleCurveData(k - 1, i, t) * (1.0 - t) + CalSingleCurveData(k - 1, i + 1, t) * t;

	}

	BazierSurface::BazierSurface() :s_DrawData(nullptr), s_Handles(nullptr) {}

	BazierSurface::BazierSurface(double* handles, int row, int col, int sample_num) : s_HandlesRowNum(row), s_HandlesColNum(col), s_SamplesNum(sample_num)
	{
		/*
		row是贝塞尔曲线数目
		col是每根贝塞尔曲线的控制点数目
		*/
		SurfaceHandles.clear();

		for (int i = 0; i < row; ++i)
		{
			std::vector<_Bhyin::Vec3> h;
			for (int j = 0; j < col; ++j)h.push_back(_Bhyin::Vec3(
				handles[i * row * 3 + j * 3],
				handles[i * row * 3 + j * 3 + 1],
				handles[i * row * 3 + j * 3 + 2]));

			SurfaceHandles.push_back(h);
		}
		GenerateSurface();
	}

	double* BazierSurface::GetHandlesData()
	{
		if (s_Handles)return s_Handles;
		else
		{
			s_Handles = new double[s_HandlesRowNum * s_HandlesColNum * 3 + 9];
			for (int i = 0; i < s_HandlesRowNum; ++i)
			{
				for (int j = 0; j < s_HandlesColNum; ++j)
				{
					s_Handles[i * s_HandlesRowNum * 3 + j * 3] = SurfaceHandles[i][j].vector()[0];
					s_Handles[i * s_HandlesRowNum * 3 + j * 3 + 1] = SurfaceHandles[i][j].vector()[1];
					s_Handles[i * s_HandlesRowNum * 3 + j * 3 + 2] = SurfaceHandles[i][j].vector()[2];
				}
			}
			return s_Handles;
		}
	}

	void BazierSurface::isHandleDragged(double x, double y)
	{
		double dx, dy;
		// 遍历所有控制点，判断是否有某个点位于x、y附近
		for (int i = 0; i < s_HandlesRowNum; ++i)
		{
			for (int j = 0; j < s_HandlesColNum; ++j)
			{
				dx = x - SurfaceHandles[i][j].vector()[0];
				dy = y - SurfaceHandles[i][j].vector()[1];
				// 距离小于某值视为被点击
				if (sqrt(dx * dx + dy * dy) < 0.1f)
				{
					isDragging = true;
					dragRowIndex = i;
					dragColIndex = j;
					break;
				}
			}
		}
	}

	void BazierSurface::RefreshSurface()
	{
		if (s_Handles)delete[]s_Handles;
		s_Handles = nullptr;
		GetHandlesData();
		if (s_DrawData)delete[]s_DrawData;
		GenerateSurface();
	}

	void BazierSurface::Drag(double x, double y)
	{
		double z = SurfaceHandles[dragRowIndex][dragColIndex].vector()[2];
		SurfaceHandles[dragRowIndex][dragColIndex] = _Bhyin::Vec3(x, y, z);
	}

	void BazierSurface::GenerateSurface()
	{
		// 计算// 计算s_HandlesRowNum条行上的贝塞尔曲线的数据
		s_Curves = new double[s_HandlesRowNum * s_SamplesNum * 3 + 1000];

		for (int i = 0; i < s_HandlesRowNum; ++i)
		{
			// 只需要曲线上的采样点，递归过程中产生的控制点数据不再需要
			SingleCurveHandles.clear();
			SingleCurveHandles = SurfaceHandles[i];

			for (int j = 0; j < s_SamplesNum; ++j)
			{
				// 计算当前帧的数据
				_Bhyin::Vec3 result = CalSingleCurveData(s_HandlesColNum - 1, 0, double(j) / s_SamplesNum);
				// 保存当前帧的数据
				s_Curves[i * s_SamplesNum * 3 + j * 3] = result.vector()[0];
				s_Curves[i * s_SamplesNum * 3 + j * 3 + 1] = result.vector()[1];
				s_Curves[i * s_SamplesNum * 3 + j * 3 + 2] = result.vector()[2];
			}
			SingleCurveHandles.clear();
		}

		s_DrawData = new double[s_SamplesNum * s_SamplesNum * 3 + 1000];

		// 计算s_SamplesNum条列上的贝塞尔曲线的数据
		for (int i = 0; i < s_SamplesNum; ++i)
		{
			// 准备控制点数据
			SingleCurveHandles.clear();
			for (int j = 0; j < s_HandlesRowNum; ++j)
			{
				SingleCurveHandles.push_back(
					_Bhyin::Vec3(
						s_Curves[j * s_SamplesNum * 3 + i * 3],
						s_Curves[j * s_SamplesNum * 3 + i * 3 + 1],
						s_Curves[j * s_SamplesNum * 3 + i * 3 + 2]
					));
			}
			SingleCurvePoints.clear();

			// 计算曲线数据
			for (int j = 0; j < s_SamplesNum; ++j)
			{
				// 计算当前帧的数据
				_Bhyin::Vec3 result = CalSingleCurveData(s_HandlesColNum - 1, 0, double(j) / s_SamplesNum);
				// 保存当前帧的数据
				s_DrawData[i * s_SamplesNum * 3 + j * 3] = result.vector()[0];
				s_DrawData[i * s_SamplesNum * 3 + j * 3 + 1] = result.vector()[1];
				s_DrawData[i * s_SamplesNum * 3 + j * 3 + 2] = result.vector()[2];
			}

			SingleCurveHandles.clear();
			SingleCurvePoints.clear();
		}
	}

	BazierSurface::~BazierSurface()
	{
		if (s_Handles)delete[]s_Handles;
		if (s_DrawData)delete[]s_DrawData;
		if (s_Curves)delete[]s_Curves;
	}

}
