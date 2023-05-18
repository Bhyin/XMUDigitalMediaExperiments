class ArithmeticCoding:
    def __init__(self):
        # 译码得到的字符序列
        self.character_sequence = []
        # 编码
        self.encoded_number = None

        # 字符数量
        self.character_number = 0
        # 字符频率
        self.character_frequency = {}

        # 获取编码间隔
        self.character_interval = {}
        # 编码涉及到浮点数运算，为了提高浮点数运算精度，使用python内置的decimal库
        self._decimal = __import__("decimal")
        # 把精度设置为50位
        self._decimal.getcontext().prec = 64
        self.D = self._decimal.Decimal

        self.C = __import__("copy")

    def compress(self, input_filepath: str, output_filepath):
        file_text = self.read(input_filepath)
        self.statistic_frequency(file_text)
        self.build_interval_dict()
        self.character_sequence = list(file_text)
        self.encode()
        self.write(output_filepath, str(self.encoded_number))

    def decompress(self, input_filepath: str, output_filepath):
        file_text = self.read(input_filepath)
        self.encoded_number = self.D(file_text)
        self.decode()
        self.write(output_filepath, ''.join(self.character_sequence))

    @staticmethod
    def read(path: str) -> str:
        try:
            with open(path, 'r+') as file:
                file_text = file.read()
        except OSError as ose:
            file_text = ""
            print(ose)
        return file_text

    @staticmethod
    def write(path: str, file_text) -> None:
        try:
            with open(path, "w") as file:
                file.write(file_text)
        except OSError as ose:
            print(ose)

    def statistic_frequency(self, file_text: str) -> None:
        """ 统计字符出现频率 """
        # 求得频数
        for char in file_text:
            self.character_frequency[char] = (self.character_frequency[char] + 1
                                              if char in self.character_frequency else 1)

        # 获取字符数量
        self.character_number = self.D(len(file_text))
        # 频数除以总数得到频率
        for char in self.character_frequency.keys():
            self.character_frequency[char] = self.D(self.character_frequency[char]) / self.character_number

    def build_interval_dict(self):
        """ 在【0,1）区间中构造子区间,即编码间隔 """
        pre_freq = self.D(0.0)
        # 按照频率，将【0,1）区间分割开来，一个字符对应一个区间，区间长度等于字符频率
        for char, freq in self.character_frequency.items():
            self.character_interval[char] = (pre_freq, pre_freq + freq)
            if pre_freq > 1.0:
                print("Error, frequency should be less than 1.0!")
            pre_freq += freq

    def encode(self):
        # 一开始的区间设置为【0,1）
        LOW = self.D(0)
        HIGH = self.D(1)
        for char in self.character_sequence:
            # 下一个事件的概率
            code_range = HIGH - LOW
            # 求得子间隔，子间隔的大小与事件的概率成比例，子间隔会成为新的“当前间隔”
            HIGH = LOW + code_range * self.character_interval[char][1]
            LOW = LOW + code_range * self.character_interval[char][0]
        # 返回区间下边界，就是给定事件序列的算术编码
        # print("encode:", LOW)
        self.encoded_number = LOW

    def decode(self):
        self.character_sequence = []
        char_num = int(self.C.deepcopy(self.character_number))
        code = self.C.deepcopy(self.encoded_number)
        while char_num:
            for char, interval in self.character_interval.items():
                if interval[0] <= code <= interval[1]:
                    self.character_sequence.append(char)
                    code -= interval[0]

                    # 将剩余部分除以子区间长度，这样就相当于把父区间又变成【0,1），译出一个字符的编码又变成未译码的编码，
                    # 就可以避免直接求子区间的子区间
                    # 每一次while循环都是在【0，1）区间内寻找编码所在的区间，译码一个字符。
                    # while循环每执行一次，译码一个字符，执行signal_length次之后，就把所有字符都译码出来。
                    code /= (interval[1] - interval[0])
                    break
            char_num -= 1


# 生成随机字符串并写入文件
def generate_random_string(length: int = 10):
    import random, string
    return ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(length))


if __name__ == '__main__':

    for i in range(10):
        random_str = generate_random_string(100)
        ArithmeticCoding.write(rf"example\{i}.txt", random_str)

    for i in range(10):
        encoder = ArithmeticCoding()
        encoder.compress(rf"example\{i}.txt", rf"compressed\file_{i}.txt")
        encoder.decompress(rf"compressed\file_{i}.txt", rf"decompressed\file_{i}.txt")
