import sys
import requests
import time
import serial
import dateutil.parser

if len(sys.argv) < 2:
    print("Usage: {} pair".format(sys.argv[0]))
    exit()

#sample_pair = 'ETH-USD'
pair = sys.argv[1]
endpointURL = 'https://api.gdax.com/products/{}/ticker'.format(pair)

ser = serial.Serial('/dev/ttyACM0',9600)

#wait for the arduino to respond
print(ser.readline())

while True:
    page = requests.get(endpointURL)
    ticker = page.json()

    price = float(ticker['price'])
    localDateTime= dateutil.parser.parse(ticker['time']).astimezone()
    date = localDateTime.strftime("%x")
    localTime = localDateTime.strftime("%I:%M:%S %p")

    print("{},{:.2f},{},{}".format(pair, price, localTime, date))
    ser.write(bytes("{},{:.2f},{},{}\x00".format(pair,price,localTime,date), 'utf-8'))

    time.sleep(5)
