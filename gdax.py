import requests
import time
import serial
from lxml import html

endpntURL = 'https://api.gdax.com/products'
testURL   = 'https://www.wikipedia.org'

page = requests.get(endpntURL)

ser = serial.Serial('/dev/ttyACM0',9600)

#wait for the arduino to respond
while not ser.readline():
    pass

pair = "ETH/USD"
price = 100.00
date = "11/12/2018"
time_pst = "11:30"

for i in range(10):
    ser.write(bytes(pair+',', 'utf-8'))
    ser.write(bytes('{:.2f},'.format(price), 'utf-8'))
    ser.write(bytes(time_pst+',', 'utf-8'))
    ser.write(bytes(date, 'utf-8'))
    ser.write(b'\x00')
    price = price + 10
    time.sleep(2)
