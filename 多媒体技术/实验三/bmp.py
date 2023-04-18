class Bmp:
    def __init__(self):
        # 文件头
        self.bfType = None
        self.bfSize = None
        self.bfReserved1 = None
        self.bfReserved2 = None
        self.bfOffBits = None

        # 信息头
        self.biSize = None
        self.biWidth = None
        self.biHeight = None
        self.biPlanes = None
        self.biBitCount = None
        self.biCompression = None
        self.biSizeImage = None
        self.biXPelsPerMeter = None
        self.biYPelsPerMeter = None
        self.biClrUsed = None
        self.biClrImportant = None

        # 24位时保存像素阵列，8位时保存颜色表+像素阵列
        self.img = None

    @staticmethod
    def i2b(number: int, length, byteorder="little"):
        """ 整数转化为长度位length的bytes """
        return number.to_bytes(length, byteorder)

    @staticmethod
    def b2i(mbytes, byteorder="little"):
        """ bytes转化为整数 """
        return int.from_bytes(mbytes, byteorder)

    def read(self, file):
        """
        读取文件
        :param file: str
        :return: int
        """
        try:
            with open(file, "rb") as file:
                # 文件头
                self.bfType = file.read(2)
                self.bfSize = file.read(4)
                self.bfReserved1 = file.read(2)
                self.bfReserved2 = file.read(2)
                self.bfOffBits = file.read(4)

                # 信息头
                self.biSize = file.read(4)
                self.biWidth = file.read(4)
                self.biHeight = file.read(4)
                self.biPlanes = file.read(2)
                self.biBitCount = file.read(2)
                self.biCompression = file.read(4)
                self.biSizeImage = file.read(4)
                self.biXPelsPerMeter = file.read(4)
                self.biYPelsPerMeter = file.read(4)
                self.biClrUsed = file.read(4)
                self.biClrImportant = file.read(4)

                # 像素阵列或者颜色表+像素阵列
                self.img = file.read()
                return 0
        except Exception as e:
            print(e)
            return -1

    def save(self, file):
        with open(file, "wb") as file:
            # 写入文件头
            file.write(self.bfType)
            file.write(self.bfSize)
            file.write(self.bfReserved1)
            file.write(self.bfReserved2)
            file.write(self.bfOffBits)

            # 写入信息头
            file.write(self.biSize)
            file.write(self.biWidth)
            file.write(self.biHeight)
            file.write(self.biPlanes)
            file.write(self.biBitCount)
            file.write(self.biCompression)
            file.write(self.biSizeImage)
            file.write(self.biXPelsPerMeter)
            file.write(self.biYPelsPerMeter)
            file.write(self.biClrUsed)
            file.write(self.biClrImportant)

            # 写入像素阵列
            file.write(self.img)

    def to_gray(self):
        """
        任务1 将24位真彩色图像 转化为 8位伪彩色灰色图像
        :return: None
        """
        img = list(self.img)
        byte_count = len(img)
        height = self.b2i(self.biHeight)
        width = self.b2i(self.biWidth)
        # width是图片的像素宽度（不一定是4的倍数），real_width是每一行实际的字节数（一定是4的倍数）
        real_width = byte_count // height

        new_img = bytes()

        # TODO 颜色表 256*4个字节长的bytes
        for i in range(256):
            new_img += (self.i2b(i, 1) + self.i2b(i, 1) + self.i2b(i, 1) + self.i2b(0, 1))

        # TODO 像素阵列， width * height * 1 个长度的bytes
        for y in range(height):
            for x in range(0, 3 * width, 3):
                # 计算对应像素的灰度值
                gray = int(0.299 * img[y * real_width + x] +
                           0.587 * img[y * real_width + x + 1] +
                           0.114 * img[y * real_width + x + 2])
                new_img += self.i2b(gray, 1)
            # 如果width不是4的倍数，就补足0，先让width对4取模，再用4减去该值，就是应当添加的0的个数
            mod4 = width % 4
            if mod4:
                for i in range(4 - mod4):
                    new_img += self.i2b(0, 1)

        # TODO 修改bitCount为8
        self.biBitCount = self.i2b(8, 2)

        # TODO 覆盖原来的像素阵列
        self.img = new_img

    def to_24bit(self):
        """
        任务二 将 8位伪彩色图像 转化为 24位真彩色图像
        :return:
        """
        img = list(self.img)

        byte_count = len(img) - 256 * 4
        height = self.b2i(self.biHeight)
        width = self.b2i(self.biWidth)
        real_width = byte_count // height

        # TODO 获取颜色表, 256组RGB值
        color_tab = img[:4 * 256]

        # TODO 利用原来的像素阵列和颜色表生成新的像素阵列
        old_pixels = img[256 * 4:]

        new_img = bytes()
        for y in range(height):
            for x in range(width):
                # 获取width和height相对应的颜色索引
                idx = old_pixels[y * real_width + x]
                r = color_tab[4 * idx]
                g = color_tab[4 * idx + 1]
                b = color_tab[4 * idx + 2]
                # print(f"R={r}, G={g}, B={b}")
                # 根据索引查找到颜色
                new_img += (self.i2b(r, 1) +
                            self.i2b(g, 1) +
                            self.i2b(b, 1))
            # 如果长度不是4的倍数，就补充0
            mod4 = (width * 3) % 4
            if mod4:
                for i in range(4 - mod4):
                    new_img += self.i2b(0, 1)

        # TODO bitCount修改位24
        self.biBitCount = self.i2b(24, 2)
        # 文件头的其他部分S
        self.biClrUsed = self.i2b(0, 4)
        offBits = 54
        SizeImage = ((((width * 24) + 31) & ~31) // 8) * height
        self.bfOffBits = self.i2b(offBits, 4)
        self.biSizeImage = self.i2b(SizeImage, 4)
        self.bfSize = self.i2b(54 + SizeImage, 4)
        # TODO 覆盖原来的颜色表和像素阵列
        self.img = new_img


if __name__ == '__main__':
    bmp = Bmp()
    # 读取
    bmp.read(r".\实验三素材\8位真彩色BMP\robert.bmp")
    # bmp.to_gray()
    bmp.to_24bit()
    # 保存
    bmp.save(r"robert.bmp")
