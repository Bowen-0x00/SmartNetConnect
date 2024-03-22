import asyncio
from bleak import BleakScanner
import time
def func(a):
    print(a)
    ...
async def main():
    start_time = time.time()
    scanner = BleakScanner(_callback=func)
    scanner._backend._callback = func
    scanner.scan_interval = 0.1

    devices = await scanner.discover(_callback=func)
    
    for d in devices:
        if d.address == '22:22:0A:6A:D8:A1':
            print(d)
    end_time = time.time()
    print(f'time: {end_time - start_time}')
asyncio.run(main())