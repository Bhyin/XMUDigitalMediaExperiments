from abc import ABC, abstractmethod


class FaceDetector(ABC):
    def __init__(self):
        self.cv = __import__("cv2")
        self.face_detector = None
        self.camera = None

    @abstractmethod
    def detect(self, gray, image):
        """ complete in subclasses """
        pass

    @abstractmethod
    def load_model(self, path: str):
        """ complete in subclasses """
        pass

    def activate_camera(self):
        # 获取索引为0的相机（第一个相机）
        self.camera = self.cv.VideoCapture(0)

    def capture(self):
        """ 捕获相机的图像并处理 """
        self.activate_camera()

        while True:
            # 读取图像，返回一个布尔值（是否成功读取一帧图像）和一个表示图像数据的numpy数组
            _, image = self.camera.read()

            # 转化为灰度图，其作用是提高数据处理速度，因为灰度图只有一个颜色通道，而rgb有3个
            gray = self.cv.cvtColor(image, self.cv.COLOR_RGB2GRAY)

            # 在灰度图上检测人脸并在图像上绘制出来
            self.detect(gray, image)

            # 显示image
            self.cv.imshow('image', image)

            # 检查是否按下了 q、Q 或者 Esc键
            if self.cv.waitKey(1) in (ord('q'), ord('Q'), 27):
                # 写入文件
                self.cv.imwrite(r"result.jpg", image)

                # post processing
                self.tear_down()

                # 关闭视频流
                break

    def tear_down(self):
        """ 关闭相机和窗口 """
        self.camera.release()
        self.cv.destroyAllWindows()


class OpenCVDetector(FaceDetector):
    """
    使用opencv检测人脸
    """

    def __init__(self):
        super().__init__()

    def load_model(self, path: str):
        """ 加载模型文件 """
        self.face_detector = self.cv.CascadeClassifier(path)

    def detect(self, gray, image):
        # 检测人脸，返回人脸矩形框的位置和宽高的列表（图像中不止一个人脸，所以用列表存储）
        faces = self.face_detector.detectMultiScale(
            gray,  # 要检测的图像
            scaleFactor=1.1,  # 每次缩放图像的比例，默认为1.1
            minNeighbors=5,  # 每个目标被检测的最小次数，默认为3
            minSize=(30, 30),  # 目标的最小尺寸，默认（30,30）
            maxSize=(3840, 2160)  # 目标的最大尺寸，默认是图像的尺寸
        )

        # 读出矩形框的位置和宽高
        for x, y, w, h in faces:
            # 绘制矩形框
            self.cv.rectangle(
                image,  # 待绘制的图像
                (x, y), (x + w, y + h),  # 对角线端点的坐标
                color=(0, 255, 0),  # 矩形的颜色
                thickness=4  # 矩形框的粗细
            )


class DlibDetector(FaceDetector):
    """
    dlib

    dlib库无法正常使用pip安装，需要先下载dlib的离线安装包，然后在下载目录下使用命令：
    `pip install dlib-19.22.99-cp310-cp310-win_amd64.whl`进行安装（注意版本）

    dlib人脸检测需使用 shape_predictor_68_face_landmarks.dat 文件
    下载链接：
    链接：https://pan.baidu.com/s/1kHqfSnsz53QnBferP_9SSg?pwd=1234
    提取码：1234
    """

    def __init__(self):
        super().__init__()
        self.dlib = __import__("dlib")
        self.predictor = None

    def load_model(self, path: str):
        """ 加载人脸检测器和68个特征点检测器 """
        self.face_detector = self.dlib.get_frontal_face_detector()
        self.predictor = self.dlib.shape_predictor(path)

    def detect(self, gray, image):
        # 识别出人脸
        faces = self.face_detector(gray)

        for face in faces:
            # 找到68个人脸特征点
            landmarks = self.predictor(gray, face)
            # 绘制出68个特征点
            for n in range(68):
                x, y = landmarks.part(n).x, landmarks.part(n).y
                self.cv.circle(
                    image,  # 待绘制的图像
                    (x, y),  # 圆心的坐标
                    2,  # 圆的半径
                    (0, 255, 0),  # 圆的颜色
                    -1  # 圆的线条粗细，设置为负数表示填充圆形而不是绘制线框
                )


if __name__ == '__main__':
    is_opencv = True
    # is_opencv = False

    opencv_requirement = r"./haarcascade_frontalface_default.xml"
    dlib_requirement = r"./shape_predictor_68_face_landmarks.dat"

    face_detector = OpenCVDetector() if is_opencv else DlibDetector()
    face_detector.load_model(opencv_requirement if is_opencv else dlib_requirement)
    face_detector.capture()
