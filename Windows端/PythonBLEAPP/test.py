import asyncio
import struct
import time

from bleak import BleakScanner, BleakClient
import uuid

# timeout_seconds = 20
# address_to_look_for = '22:22:0A:6A:D8:A1'
# service_id_to_look_for = '0000feaa-0000-1000-8000-00805f9b34fb'
loop = None

class MyScanner:
    def __init__(self):
        self._scanner = BleakScanner()
        # self._scanner.scan_interval = 0.1
        self._scanner.register_detection_callback(self.detection_callback)
        self.scanning = asyncio.Event()
        self.device = None

    def detection_callback(self, device, advertisement_data):
        # if device.address == address_to_look_for:
        if list(advertisement_data.service_data.values())[0] == b'\x55\x66\x77\x88':
            # print(device)
            self.device = device 
            print(advertisement_data, time.ctime())
            if advertisement_data.service_data:
                print(advertisement_data)
                self.scanning.clear()

    async def run(self):
        start_time = time.time()
        await self._scanner.start()
        self.scanning.set()
        while self.scanning.is_set():
            await asyncio.sleep(0.1)
        end_time = time.time()
        print(f'time: {end_time - start_time}')
        await self._scanner.stop()
    
        async with BleakClient(address_or_ble_device=self.device, loop=loop) as client:
            # await client.connect()
            # a = client.services
            # for b in a:
            #     for c in b.characteristics:
            #         print(c)
            await client.write_gatt_char(uuid.UUID('{00002223-0000-1000-8000-00805f9b34fb}'), bytes('Shadow\n12345678r', encoding = "utf8"), False)
        


if __name__ == '__main__':
    my_scanner = MyScanner()
    loop = asyncio.get_event_loop()
    loop.run_until_complete(my_scanner.run())