import json
import os
import re


def find_values_by_key(data, target_key):
    results = []

    if isinstance(data, dict):
        for key, value in data.items():
            if key == target_key:
                results.append(value)
            results.extend(find_values_by_key(value, target_key))
    elif isinstance(data, list):
        for item in data:
            results.extend(find_values_by_key(item, target_key))
    return results


json_file_path = os.path.join(os.path.dirname(__file__), 'lyric.json')
if os.path.exists(json_file_path):
    with open(json_file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
else:
    print("JSON 文件不存在！")

result = find_values_by_key(data, "lyric")


# 清除控制台 ANSI 转义
def clean_output(text):
    ansi_escape = re.compile(r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])')
    return ansi_escape.sub('', text)


if result:
    for i, lyric in enumerate(result):
        print(f"\n{'=' * 60}")
        print(f"歌词 {i + 1}:")
        print('=' * 60)
        # 将 \n 转换为实际换行
        formatted_lyric = lyric.replace('\\n', '\n')
        # 清除 ANSI 转义序列
        cleaned_lyric = clean_output(formatted_lyric)

        # 输出歌词
        print(cleaned_lyric)

        # 保存到文件
        file_name = f"{i + 1}.lrc"
        with open(file_name, 'w', encoding='utf-8') as f:
            f.write(cleaned_lyric)
else:
    print("未找到 lyric 字段！")

