# 텍스트 파일 열기
with open('파일경로/파일이름.txt', 'r') as file:
    # 파일 내용 읽기
    content = file.read()
    # 또는 한 줄씩 읽기
    # line = file.readline()
    # 또는 모든 줄을 리스트로 읽기
    # lines = file.readlines()

# 파일 내용 출력
print(content)
