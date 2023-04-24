import heapq
import os


def INFO(info):
    print(f'\033[32m INFO: {info}\033[0m')


def ERROR(error):
    print(f'\033[31m ERROR: {error}\033[0m')


class HuffmanCoding:
    def __init__(self, path_: str):
        self.path: str = path_
        self.priority_queue: list = []
        self.codes: dict = {}

    class HeapNode:
        def __init__(self, char: str, freq: int):
            self.char: str = char
            self.frequency: int = freq
            self.left = None
            self.right = None

        def __lt__(self, other: "HeapNode") -> bool:
            """ 小于符号重载 """
            return self.frequency < other.frequency if self._valuable(other) else False

        def __eq__(self, other: "HeapNode") -> bool:
            """ 等于符号重载 """
            return self.frequency == other.frequency if self._valuable(other) else False

        def _valuable(self, node):
            return node is not None and isinstance(node, self.__class__)

        def __repr__(self):
            return f"\n\tNode [character: {self.char},\t\tfrequency: {self.frequency}]"

        def __str__(self):
            return f"Node Info: char->{self.char}, frequency->{self.frequency}"

    @staticmethod
    def statistic_frequency(text: str) -> dict:
        """ 统计字符出现频率 """
        frequency = {}
        for char in text:
            frequency[char] = frequency[char] + 1 if char in frequency else 1
        frequency["EOF"] = 1
        INFO("Statistic frequency successfully: " + repr(frequency))
        return frequency

    def build_priority_queue(self, frequency: dict) -> None:
        """ 建立优先队列 """
        for char in frequency:
            heapq.heappush(self.priority_queue, self.HeapNode(char, frequency[char]))
        INFO("Build priority queue successfully :" + repr(self.priority_queue))

    def build_huffman_tree(self) -> None:
        """ 构造霍夫曼树 """
        while len(self.priority_queue) > 1:
            # pop最小的两个节点
            node1 = heapq.heappop(self.priority_queue)
            node2 = heapq.heappop(self.priority_queue)

            # 合并节点
            merged = self.HeapNode("", node1.frequency + node2.frequency)
            merged.left, merged.right = node1, node2

            # 重新push
            heapq.heappush(self.priority_queue, merged)
        INFO("Build Huffman tree successfully")

    def search_huffman_tree(self, root: HeapNode, current_code: str) -> None:
        """ 遍历霍夫曼树 """
        if root is None:
            return

        # 遍历到叶子节点，将得到的编码存储起来
        if root.char != "":
            self.codes[root.char] = current_code
            return

        # 左孩子0，右孩子1
        self.search_huffman_tree(root.left, current_code + '0')
        self.search_huffman_tree(root.right, current_code + '1')

    def encode(self) -> None:
        """ 获取霍夫曼编码 """
        self.search_huffman_tree(heapq.heappop(self.priority_queue), "")
        INFO("Encode successfully: " + repr(self.codes))

    def get_encode_text(self, text: str) -> str:
        """ 获取编码后的0-1字符串 """
        encoded_text = ""
        for char in text:
            encoded_text += self.codes[char]
        encoded_text += self.codes["EOF"]
        INFO("Get encoded text: " + encoded_text)
        return encoded_text

    def compress(self, output_name: str):
        output_name += '.txt'

        try:
            with open(self.path, 'r+') as file, open(output_name, 'w+') as output:
                INFO('\n' + '-+' * 20 + '\n')
                INFO("Open file " + self.path)
                text = file.read()

                frequency = self.statistic_frequency(text)
                self.build_priority_queue(frequency)
                self.build_huffman_tree()
                self.encode()

                encoded_text = self.get_encode_text(text)

                output.write(encoded_text)
                INFO("Write encoded text!")
                INFO("Output file is " + output_name)
        except OSError as e:
            ERROR(repr(e))

        return output_name


if __name__ == '__main__':
    input_dir = r"D:\Users\Bhyin\Desktop\text file"

    for path in os.listdir(input_dir):
        file_name, extension = os.path.splitext(path)
        huffman_encoder = HuffmanCoding(os.path.join(input_dir, path))
        huffman_encoder.compress(f"encoded_{file_name}")
