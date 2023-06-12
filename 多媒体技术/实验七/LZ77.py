def encode(file_text, window_size=5):
    """ LZ77编码
    :param file_text: 读取出的字符串
    :param window_size: 滑动窗口的长度
    :return: 编码好的二进制序列
    """
    encoded_data = bytearray()

    # 窗口的位置
    window_position = 0

    # 字符串长度
    file_text_len = len(file_text)

    while window_position < file_text_len:
        # 当前窗口最长匹配字符的长度
        length = 0
        # 匹配字符串最大的偏移
        offset = 0

        # 比较未编码数据和窗口中的各个字符，寻找最大匹配
        for char_offset in range(1, min(window_size, window_position + 1)):

            # 开始匹配，如果当前压缩位置中的字符与窗口中的字符匹配，则k+1匹配下一个
            max_match_len = 0
            while (max_match_len < file_text_len - window_position and
                   file_text[window_position + max_match_len] ==
                   file_text[window_position - char_offset + max_match_len]):
                max_match_len += 1

            # 如果k更大，说明匹配到更长字符串，修改匹配长度和偏移
            if max_match_len > length:
                length, offset = max_match_len, char_offset

        # 确保window_pos+length不会超过字符串长度
        if window_position + length >= file_text_len:
            length = file_text_len - window_position - 1

        # 写入偏移、长度和下一个字符的数据
        encoded_data.append(offset)
        encoded_data.append(length)
        encoded_data.append(ord(file_text[window_position + length]))

        # 窗口向后移动length+1
        window_position += (length + 1)

    return encoded_data


def decode(encoded_data):
    decoded_data = ""

    encoded_data_len = len(encoded_data)

    for i in range(0, encoded_data_len, 3):
        # 读出三元组数据
        offset, length, next_char = encoded_data[i], encoded_data[i + 1], chr(encoded_data[i + 2])
        # 用匹配字符串拼接上next char
        decoded_data += (decoded_data[-offset:][:length] + next_char)

    return decoded_data


def compress(input_file, output_file, window_size):
    # 读取
    with open(input_file, 'r') as file:
        file_text = file.read()

    # 编码
    encoded_data = encode(file_text, window_size)

    # 存储
    with open(output_file, 'wb') as file:
        file.write(encoded_data)


def decompress(input_file, output_file):
    # 读取
    with open(input_file, 'rb') as file:
        encoded_data = file.read()

    # 解码
    decoded_data = decode(encoded_data)

    # 存储
    with open(output_file, 'w') as file:
        file.write(decoded_data)


if __name__ == '__main__':
    import time
    import os

    input_dir = r".\text_files"
    temp_dir = r".\encoded_data"
    output_dir = r".\decoded_data"

    for file in os.listdir(input_dir):
        start = time.time()
        compress(
            os.path.join(input_dir, file),
            os.path.join(temp_dir, file.split('.')[0]),
            5
        )
        compress_time = time.time() - start
        print(f"压缩{file}，耗时{compress_time}秒")

        start = time.time()
        decompress(
            os.path.join(temp_dir, file.split('.')[0]),
            os.path.join(output_dir, file)
        )
        decompress_time = time.time() - start
        print(f"解压{file} 耗时{decompress_time}秒")
        print()
