## 각종 함수 선언 및 초기 세팅

import random, turtle, winsound
from turtle import *

screen = turtle.Screen( )
turtle.setup(width=800, height=600)
winsound.PlaySound('img/back.wav', winsound.SND_FILENAME | winsound.SND_ASYNC | winsound.SND_NOSTOP)
screen.bgpic('img/back0.gif')
screen.bgpic('img/back1.gif')
screen.bgpic('img/back2.gif')

## 전역 변수로 이미지 트래킹을 위한
## 거북이 10개 만들기

for i in range(1, 11):
    globals( )["p{}".format(i)] = "img\p" + str(i) + ".gif"
    globals( )["t{}".format(i)] = turtle.Turtle( )

for i in range(1, 11):
    screen.addshape(eval("p" + str(i)))
    eval("t" + str(i)).shape(eval("p" + str(i)))
    eval("t" + str(i)).pu( )
    eval("t" + str(i)).speed(5)

## 셔플 애니메이션

for i in range(180):
    for i in range(1, 11):
        turtle.delay(0)
        eval("t" + str(i)).shape(eval("p" + str(i))), eval("t" + str(i)).circle(100, (2 * i))

turtle.delay(10)
for i in range(1, 11):
    eval("t" + str(i)).speed(5)

pepe, pe = [3, 3, 2, 2, 2, 2, 1, 1, 1, 1], []
card1, card2 = [], []
tur1, tur2 = [], []

random.shuffle(pepe)
pe = sorted(pepe, reverse=True)

## pepe는 셔플 완료된 랜덤 값
## pe는 순수 값이 됨

## 섞인 카드 배분하기

winsound.PlaySound(None, winsound.SND_ASYNC)
screen.bgpic('img/back1.gif')

i = 0
while i < 10:
    aaa = random.choice(['앞', '뒤'])
    if aaa == '앞':
        if len(card1) == 5:
            continue
        else:
            card1.append(pe[i])
            i += 1
            eval("t" + str(11 - i)).goto(i * 30, 200)
            winsound.PlaySound('img/slash.wav', winsound.SND_FILENAME | winsound.SND_NOSTOP)
            tur1.append(11 - i)
    else:
        if len(card2) == 5:
            continue
        else:
            card2.append(pe[i])
            i += 1
            eval("t" + str(11 - i)).goto(i * 30, -200)
            winsound.PlaySound('img/slash.wav', winsound.SND_FILENAME | winsound.SND_NOSTOP)
            tur2.append(11 - i)
delay(10)

## 완성된 카드덱 흔들기 애니메이션

screen.bgpic('img/back3.gif')
for i in range(4):
    for i in range(1, 11):
        turtle.delay(0)
        eval("t" + str(i)).circle(5, 540)
turtle.delay(10)

## 게임 시작하기

d, s, s1, s2 = -200, 0, sum(card1), sum(card2)
ss = (s1 - s2)

while len(card2) != 0:
    it1 = int(input("-_-_" * 10 + "\n보유 중인 카드 중에서 숫자만 입력하세요 (잼민이 : 3, 부자 : 2, 경찰 : 1)\n"))
    if it1 in card1:
        print("보유 중인 카드이며 선택된 카드는", it1, "입니다.")
        screen.bgpic('img/back1.gif')

        # 카드합산에서 내가 유리

        if ss >= 0:
            print("____" * 10)
            for j in range(0, len(card2)):
                if int(it1) > int(card2[j]):

                    # event1
                    eval("t" + str(tur1[card1.index(it1)])).goto(d, random.randint(10, 20))
                    winsound.PlaySound('img/slash.wav', winsound.SND_FILENAME | winsound.SND_NOSTOP)
                    d += 20
                    del tur1[card1.index(it1)]
                    card1.remove(it1)

                    print("★ 나의 승리!! 봇의 선택은...", card2[j],
                          "\n 스코어 획득에 성공했습니다!!")
                    s += 1

                    eval("t" + str(tur2[card2.index(card2[j])])).goto(d, -random.randint(10, 20))
                    winsound.PlaySound('img/slash.wav', winsound.SND_FILENAME | winsound.SND_NOSTOP)
                    d += 20
                    del tur2[j]
                    card2.remove(card2[j]), print(card2)

                    print('event2')
                    break

                else:
                    # event2
                    eval("t" + str(tur1[card1.index(it1)])).goto(d, random.randint(10, 20))
                    winsound.PlaySound('img/slash.wav', winsound.SND_FILENAME | winsound.SND_NOSTOP)
                    d += 20
                    del tur1[card1.index(it1)]
                    card1.remove(it1)

                    print("나의 무승부 혹은 패배... 봇의 선택은...", card2[j],
                          "\n 스코어 획득에 실패했습니다...")

                    eval("t" + str(tur2[card2.index(card2[j])])).goto(d, -random.randint(10, 20))
                    winsound.PlaySound('img/slash.wav', winsound.SND_FILENAME | winsound.SND_NOSTOP)
                    d += 20
                    del tur2[j]
                    card2.remove(card2[j]), print(card2)

                    print('event1')
                    break

        else:

            # 카드합산에서 상대방이 유리

            print("___" * 20)
            for j in range(0, len(card2)):
                if int(it1) > int(card2[j]):
                    # event3
                    eval("t" + str(tur1[card1.index(it1)])).goto(d, random.randint(10, 20))
                    winsound.PlaySound('img/slash.wav', winsound.SND_FILENAME | winsound.SND_NOSTOP)
                    d += 20
                    del tur1[card1.index(it1)]
                    card1.remove(it1)

                    print("★ 나의 승리!! 봇의 선택은...", card2[j],
                          "\n 스코어 획득에 성공했습니다!!")
                    s += 1

                    eval("t" + str(tur2[card2.index(card2[j])])).goto(d, -random.randint(10, 20))
                    winsound.PlaySound('img/slash.wav', winsound.SND_FILENAME | winsound.SND_NOSTOP)
                    d += 20
                    del tur2[j]
                    card2.remove(card2[j]), print(card2)

                    print('event3')
                    break
                else:
                    # event4
                    eval("t" + str(tur1[card1.index(it1)])).goto(d, random.randint(10, 20))
                    winsound.PlaySound('img/slash.wav', winsound.SND_FILENAME | winsound.SND_NOSTOP)
                    d += 20
                    del tur1[card1.index(it1)]
                    card1.remove(it1)

                    print("나의 무승부 혹은 패배... 봇의 선택은...", card2[j],
                          "\n 스코어 획득에 실패했습니다...")

                    eval("t" + str(tur2[card2.index(card2[j])])).goto(d, -random.randint(10, 20))
                    winsound.PlaySound('img/slash.wav', winsound.SND_FILENAME | winsound.SND_NOSTOP)
                    d += 20
                    del tur2[j]
                    card2.remove(card2[j]), print(card2)

                    print('event4')
                    break
    else:
        print("존재하지 않는 숫자", it1)

##스코어 s 값 따라서 승패여부 결과도출

if s > 2:
    print("****" * 10)
    winsound.PlaySound('img/win.wav', winsound.SND_FILENAME | winsound.SND_ASYNC | winsound.SND_NOSTOP)
    print("당신은 5번 중에서 총", s, "번 이겨서 승리하였습니다 !!!")
    screen.bgpic('img/back4.gif')
else:
    print("****" * 10)
    winsound.PlaySound('img/lose.wav', winsound.SND_FILENAME | winsound.SND_ASYNC | winsound.SND_NOSTOP)
    print("당신은 5번 중에서 총", s, "번 밖에 못 이겨서 패배하였습니다...")
    screen.bgpic('img/back5.gif')
