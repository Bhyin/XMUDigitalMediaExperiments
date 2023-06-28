#pragma once

namespace Bhyin
{
	class BazierCurve
	{
	public:
		BazierCurve();
		BazierCurve(double* handles, int N, int sample_num = 1000);

		~BazierCurve();

		// 添加顶点
		void AppendHandle(double x, double y, double z);

		// 清除数据
		void ClearDrawData();
		void ClearAllData();

		// 重新生成数据
		void RefreshCurve();


		inline void SetSamplesNum(int n) { s_SampleNum = n; }

		// 传递给glut的数据
		inline int GetHandlesNum() { return s_HandlesNum; }

		inline int GetSamplesNum() { return s_SampleNum; }

		int GetFrameSize();

		void isHandleDragged(double x,double y);
		
		void Drag(double x,double y);

		double* GetHandlesData();
		double* GetCurvesData();
		inline double* GetFrameData(int frame, int k) { return s_DrawData + frame * GetFrameSize() * 3 + GetFrameOffset(k); }
		inline double* GetCurveData() { return s_DrawData + GetFrameOffset(s_HandlesNum - 1); }

	protected:
		inline int Func1(int m, int n) { return n <= 1 ? 0 : m + Func1(m - 1, n - 1); }
	private:
		inline int GetFrameOffset(int k) { return 3 * Func1(s_HandlesNum - 1, k); }

		// 生成曲线数据
		void GenerateCurve();

	public:
		// 曲线是否存在
		bool exist;
		// 是否被拖动
		bool isDragging;

		int dragIndex;

	private:
		// 控制点数组
		double* s_Handles;

		// 控制点数量
		int s_HandlesNum;

		// 采样次数
		int s_SampleNum;

		// 每次采样计算的点数目
		int s_FrameSize;


		// DrawData
		/*
		一个vector，保存若干个double指针，每个指针指向一个一维double数组
		从头到尾依次存放在递归时每一层产生的点
		第一个指针是贝塞尔曲线上的采样点，最后一个指针就是控制点
		*/
		double* s_DrawData;
	};

	// 贝塞尔曲面
	class BazierSurface
	{
	public:
		BazierSurface();

		BazierSurface(double* handles, int row, int col, int sample_num);

		~BazierSurface();

		// 重新生成数据
		void RefreshSurface();

		inline int GetHandlesRowNum() { return s_HandlesRowNum; }
		inline int GetHandlesColNum() { return s_HandlesColNum; }
		inline int GetSamplesNum() { return s_SamplesNum; }

		double* GetHandlesData();

		inline double* GetCurvesData() { return s_Curves; }

		inline double *GetSurfaceData() { return s_DrawData; }

		void isHandleDragged(double x, double y);

		void Drag(double x, double y);
	private:
		void GenerateSurface();
	public:

		// 是否被拖动
		bool isDragging;

		// 被拖动的点在哪一行哪一列
		int dragRowIndex;
		int dragColIndex;

	private:
		// 所有控制点
		double* s_Handles;
		// 控制点行数
		int s_HandlesRowNum;
		// 控制点列数
		int s_HandlesColNum;
		// 采样数目
		int s_SamplesNum;
		// 曲面顶点数据
		double* s_DrawData;

		double* s_Curves;
	};
}


