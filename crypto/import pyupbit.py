import pyupbit

KRW = pyupbit.get_tickers(fiat="KRW")
BTC = pyupbit.get_tickers(fiat="BTC")

# g = pyupbit.get_ohlcv()

array1 = list(set(i[4:] for i in BTC) ^ set(j[4:] for j in KRW))
array2 = list(set(i[4:] for i in BTC) - set(j[4:] for j in KRW))

array2 = sorted(array2, reverse=False)

output_string = "BTC, \n".join("UPBIT:" + str(item) for item in array2) + "BTC"

print(output_string)
# print(g)