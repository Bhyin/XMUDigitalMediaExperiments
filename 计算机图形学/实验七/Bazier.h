#pragma once

namespace Bhyin
{
	class BazierCurve
	{
	public:
		BazierCurve();
		BazierCurve(double* handles, int N, int sample_num = 1000);

		~BazierCurve();

		// ��Ӷ���
		void AppendHandle(double x, double y, double z);

		// �������
		void ClearDrawData();
		void ClearAllData();

		// ������������
		void RefreshCurve();


		inline void SetSamplesNum(int n) { s_SampleNum = n; }

		// ���ݸ�glut������
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

		// ������������
		void GenerateCurve();

	public:
		// �����Ƿ����
		bool exist;
		// �Ƿ��϶�
		bool isDragging;

		int dragIndex;

	private:
		// ���Ƶ�����
		double* s_Handles;

		// ���Ƶ�����
		int s_HandlesNum;

		// ��������
		int s_SampleNum;

		// ÿ�β�������ĵ���Ŀ
		int s_FrameSize;


		// DrawData
		/*
		һ��vector���������ɸ�doubleָ�룬ÿ��ָ��ָ��һ��һάdouble����
		��ͷ��β���δ���ڵݹ�ʱÿһ������ĵ�
		��һ��ָ���Ǳ����������ϵĲ����㣬���һ��ָ����ǿ��Ƶ�
		*/
		double* s_DrawData;
	};

	// ����������
	class BazierSurface
	{
	public:
		BazierSurface();

		BazierSurface(double* handles, int row, int col, int sample_num);

		~BazierSurface();

		// ������������
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

		// �Ƿ��϶�
		bool isDragging;

		// ���϶��ĵ�����һ����һ��
		int dragRowIndex;
		int dragColIndex;

	private:
		// ���п��Ƶ�
		double* s_Handles;
		// ���Ƶ�����
		int s_HandlesRowNum;
		// ���Ƶ�����
		int s_HandlesColNum;
		// ������Ŀ
		int s_SamplesNum;
		// ���涥������
		double* s_DrawData;

		double* s_Curves;
	};
}


