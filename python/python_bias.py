import random

class NumberBiasPy:
    def __init__(self, favorite_number, bias_factor=2):
        self.favorite_number = favorite_number
        self.bias_factor = bias_factor

    def generate_number(self):
        # 1부터 10까지의 숫자를 생성
        number = random.randint(1, 10)

        # 생성된 숫자가 좋아하는 숫자와 같으면 확률을 높여줌
        if number == self.favorite_number:
            biased_probability = random.uniform(0, 1)
            if biased_probability > 0.5:  # 임의로 설정한 임계값
                return number

        # 좋아하는 숫자가 아니거나 높은 확률로 반환하지 않는 경우
        return random.randint(1, 10)

# 좋아하는 숫자를 7로 설정한 경우
my_bias_py = NumberBiasPy(favorite_number=7)

# 여러 번 숫자를 생성해봅니다.
for _ in range(10):
    generated_number = my_bias_py.generate_number()
    print(generated_number)
