import turtle

# 터틀 화면과 터틀 객체 생성
screen = turtle.Screen()
pen = turtle.Turtle()

# 정사각형 그리기
for _ in range(4):
    pen.forward(100)  # 거북이를 100 단위만큼 전진시킴
    pen.right(90)     # 거북이를 90도 오른쪽으로 회전시킴

# 마우스 클릭 시 터틀 그래픽 창 닫기
screen.exitonclick()

