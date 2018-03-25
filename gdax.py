import requests
import time
import serial
from lxml import html
import dateutil.parser

pair = 'ETH-USD'
endpntURL = 'https://api.gdax.com/products/{}/ticker'.format(pair)

ser = serial.Serial('/dev/ttyACM0',9600)

#wait for the arduino to respond
while not ser.readline():
    pass

while(True):
    print('loop: {}'.format(i))
    page = requests.get(endpntURL)
    ticker = eval(page.text)

    price = float(ticker['price'])
    datetime = str(dateutil.parser.parse(ticker['time']).astimezone())
    date = datetime[:datetime.index(' ')]
    time_pst = datetime[datetime.index(' ') + 1:datetime.index('.')]
    

    print("{},{:.2f},{},{}\x00".format(pair,price,time_pst,date))
    ser.write(bytes("{},{:.2f},{},{}\x00".format(pair,price,time_pst,date), 'utf-8'))

    time.sleep(5)
