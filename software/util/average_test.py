
import random
import math


data = []

INT_ALPHA = 77


for x in range(0, 5):
	data.append(190+random.random()*80)



def exponential_moving_average_fp(value, average, alpha):
	new_average = alpha*value + (1-alpha) * average
	return new_average


def exponential_moving_average_int(value, average, alpha):
	new_average = alpha*value + (65536 -alpha) * average
	new_average = (new_average + 32768) / 65536 
	return new_average	


total = 0
for value in data:
	total = total + value;

average = total / len(data)
print(average)

average_ema = 0
for value in data:
	average_ema = exponential_moving_average_fp(value, average_ema, 0.3)

print(average_ema)

print("Error percentage float:",(abs(average-average_ema)/average)*100)


average_ema = 0
for value in data:
	average_ema = exponential_moving_average_int(value, average_ema, 19660)

print(average_ema)
print("Error percentage fix:",(abs(average-average_ema)/average)*100)



