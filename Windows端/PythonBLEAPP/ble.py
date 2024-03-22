import asyncio
from bleak import BleakScanner

async def run():
    devices = await BleakScanner.discover()
    for d in devices:
        if d.address == '22:22:0A:6A:D8:A1':
            print(d)
            if d.metadata:
                print(d.metadata)

loop = asyncio.get_event_loop()
loop.run_until_complete(run())