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

		// ʵ�ֻ����ļӷ�����������
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

	// ����i��k����Ψһȷ��һ�����㣬������һ��ɢ�б�洢
	std::map<std::pair<int, int>, _Bhyin::Vec3> CurvePoints;

	// ���Ƶ�
	std::vector<_Bhyin::Vec3> CurveHandles;

	// de Castejau�㷨
	_Bhyin::Vec3 de_Castejau(int k, int i, double t)
	{
		// ���
		_Bhyin::Vec3 result;

		// ��
		auto p = std::pair<int, int>(k, i);
		// �Ƿ��Ѿ������
		auto it = CurvePoints.find(p);
		// ���û�����
		if (it == CurvePoints.end())
		{
			//��kΪ0��ֱ�ӷ��ض�Ӧ���Ƶ㣬����ݹ�
			// ���⣬��kΪ0���Ͳ���Points�ֵ���涥����
			if (k == 0)result = CurveHandles[i];
			else
			{
				result = de_Castejau(k - 1, i, t) * (1.0 - t) + de_Castejau(k - 1, i + 1, t) * t;
				CurvePoints[p] = result;// �Ѽ������ֵ����
			}

		}
		else result = CurvePoints[p]; // ���������ֱ�ӷ��ؼ������ֵ
		/*printf("i: %d, k: %d, vec:(%f, %f, %f)\n", i, k, result.vector()[0], result.vector()[1], result.vector()[2]);*/
		//���ؽ��
		return result;
	}

	BazierCurve::BazierCurve() :exist(false), s_HandlesNum(0), s_SampleNum(200), s_FrameSize(-1), s_Handles(nullptr), s_DrawData(nullptr)
	{
	}

	BazierCurve::BazierCurve(double* handles, int N, int sample_num) : exist(false), s_HandlesNum(N), s_SampleNum(sample_num), s_FrameSize(-1), s_Handles(nullptr), s_DrawData(nullptr)
	{
		exist = s_HandlesNum > 0;
		// ��ʼ�����Ƶ�
		for (int i = 0; i < s_HandlesNum; ++i)
			CurveHandles.push_back(_Bhyin::Vec3{ handles[3 * i], handles[3 * i + 1], handles[3 * i + 2] });

		// ��������
		if (s_HandlesNum != 0)RefreshCurve();
	}

	void BazierCurve::AppendHandle(double x, double y, double z)
	{
		// ��ӿ��Ƶ�
		CurveHandles.push_back(_Bhyin::Vec3{ x,y,z });
		s_HandlesNum += 1;
		exist = true;

		RefreshCurve();
	}

	void BazierCurve::RefreshCurve()
	{
		s_FrameSize = -1; // ��ÿһ֡����Ŀ��Ƶ�������������ʼ״̬

		//���⻹Ӧ�����ԭ�����ݣ��������Ѿ���GenerateCurve�е���ClearDataʵ���ˡ�

		// ���¼�������
		GenerateCurve();
	}

	void BazierCurve::GenerateCurve()
	{
		// ��ʼ��DrawData,ÿ������3��������ÿ�β�����Ҫ(s_HandlesNum + 1) * s_HandlesNum / 2�����㣬������s_SampleNum��
		// ����ÿһ�μ��㣬���Ƶ��ֵ���ǲ���ģ������Ѿ�������s_Handles���ˣ����Դ�DrawData��ʡȥ��
		// ����һ�����Լ��� s_HandlesNum * s_SampleNum ��double�Ĵ洢�ռ�
		// ʽ�ӿ����޸�����
		ClearDrawData();

		s_DrawData = new double[3 * GetFrameSize() * s_SampleNum + 200];

		// һЩ��ҪƵ�����������ٵı������ƶ���ѭ��֮��
		int pointNum = 0, frameIdx = 0, pointIdx = 0, j = 0;
		double* temp;

		// ���㱴�����������ɹ��̲��������еĵ�
		for (int i = 0; i < s_SampleNum; ++i)
		{
			CurvePoints.clear(); // ����ֵ�

			de_Castejau(s_HandlesNum - 1, 0, double(i) / s_SampleNum);
			//���ֵ��е�ֵת�Ƶ�DrawData��
			/*mapĬ�������Լ�Ϊ��׼��������pair�����ȱȽϵ�һ��Ԫ���ٱȽϵڶ���Ԫ��
			����map�е�Vec�������(0,0),(0,1),...,(1,0),(1,1),...,(2,0),(2,1),...��˳������
			����ת�Ƶ�DrawData�ϣ�Ȼ��ʹ��glDrawArray�����ƣ��Ƚ����׿���
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
		// �������п��Ƶ㣬�ж��Ƿ���ĳ����λ��x��y����
		for (int i = 0; i < s_HandlesNum; ++i)
		{
			dx = x - CurveHandles[i].vector()[0];
			dy = y - CurveHandles[i].vector()[1];
			// ����С��ĳֵ��Ϊ�����
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


	/**********************************�����Ǳ���������************************************/
	/**********************************�����Ǳ���������************************************/
	/**********************************�����Ǳ���������************************************/
	// ������������Ŀ��Ƶ������
	std::vector<std::vector<_Bhyin::Vec3>> SurfaceHandles;

	// �����������е����������ж�������
	std::map<std::pair<int, int>, _Bhyin::Vec3> SingleCurvePoints;
	// �����������е������ߵĿ��Ƶ�����
	std::vector<_Bhyin::Vec3>  SingleCurveHandles;

	// ���㵥�����ߵ�����
	_Bhyin::Vec3 CalSingleCurveData(int k, int i, double t)
	{
		return k == 0 ? SingleCurveHandles[i] : CalSingleCurveData(k - 1, i, t) * (1.0 - t) + CalSingleCurveData(k - 1, i + 1, t) * t;

	}

	BazierSurface::BazierSurface() :s_DrawData(nullptr), s_Handles(nullptr) {}

	BazierSurface::BazierSurface(double* handles, int row, int col, int sample_num) : s_HandlesRowNum(row), s_HandlesColNum(col), s_SamplesNum(sample_num)
	{
		/*
		row�Ǳ�����������Ŀ
		col��ÿ�����������ߵĿ��Ƶ���Ŀ
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
		// �������п��Ƶ㣬�ж��Ƿ���ĳ����λ��x��y����
		for (int i = 0; i < s_HandlesRowNum; ++i)
		{
			for (int j = 0; j < s_HandlesColNum; ++j)
			{
				dx = x - SurfaceHandles[i][j].vector()[0];
				dy = y - SurfaceHandles[i][j].vector()[1];
				// ����С��ĳֵ��Ϊ�����
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
		// ����// ����s_HandlesRowNum�����ϵı��������ߵ�����
		s_Curves = new double[s_HandlesRowNum * s_SamplesNum * 3 + 1000];

		for (int i = 0; i < s_HandlesRowNum; ++i)
		{
			// ֻ��Ҫ�����ϵĲ����㣬�ݹ�����в����Ŀ��Ƶ����ݲ�����Ҫ
			SingleCurveHandles.clear();
			SingleCurveHandles = SurfaceHandles[i];

			for (int j = 0; j < s_SamplesNum; ++j)
			{
				// ���㵱ǰ֡������
				_Bhyin::Vec3 result = CalSingleCurveData(s_HandlesColNum - 1, 0, double(j) / s_SamplesNum);
				// ���浱ǰ֡������
				s_Curves[i * s_SamplesNum * 3 + j * 3] = result.vector()[0];
				s_Curves[i * s_SamplesNum * 3 + j * 3 + 1] = result.vector()[1];
				s_Curves[i * s_SamplesNum * 3 + j * 3 + 2] = result.vector()[2];
			}
			SingleCurveHandles.clear();
		}

		s_DrawData = new double[s_SamplesNum * s_SamplesNum * 3 + 1000];

		// ����s_SamplesNum�����ϵı��������ߵ�����
		for (int i = 0; i < s_SamplesNum; ++i)
		{
			// ׼�����Ƶ�����
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

			// ������������
			for (int j = 0; j < s_SamplesNum; ++j)
			{
				// ���㵱ǰ֡������
				_Bhyin::Vec3 result = CalSingleCurveData(s_HandlesColNum - 1, 0, double(j) / s_SamplesNum);
				// ���浱ǰ֡������
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
