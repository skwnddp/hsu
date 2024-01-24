import requests

def get_country_info(country_name):
    url = f'https://restcountries.com/v3.1/name/{country_name}'
    response = requests.get(url)

    if response.status_code == 200:
        country_info = response.json()[0]
        return country_info
    else:
        return None

# 대한민국 정보 얻기
south_korea_info = get_country_info('South Korea')

# 출력
if south_korea_info:
    print(f"대한민국 정보:\n{south_korea_info}")
else:
    print("대한민국 정보를 가져오는 데 실패했습니다.")
