
def dataToHex(text: str | list):
    hex_list = list(map(lambda c: '{:02X}'.format(c), text))
    return (hex_list)

def strToHex(text: str):
    hex_list = ', '.join(list(map(lambda c: '0x{:02X}'.format(ord(c)), text)))
    return (hex_list)

def genrate_ad_structure(data: list):
    result = ''
    hex_data = dataToHex(data)
    # if len(hex_data) < 32:
    #     hex_data += ' ' * 32 - len(hex_data)
    result += '{:02X} '.format(len(data)) + ' '.join(hex_data)
    return result

def genrate_ad_str(data_list: list):
    result = '0x08 0x0009 '
    count = 0
    data_str = ''
    for l in data_list:
        data_str += genrate_ad_structure(l) + ' '
        count += len(l) + 1
    result += '{:02X} '.format(count)
    result += data_str
    if count < 31:
       result += '00 ' *  (32 - count - 1)

    return result

if __name__ == '__main__':
    # print(genrate_ad_str([[0x30, 0x6A, 0x75, 0x73, 0x74, 0x2D, 0x73, 0x74, 0x6F, 0x70, 0x2D, 0x69, 0x74]]))
    print(genrate_ad_str([[0x20, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88]]))


    # print(genrate_ad_str([[0x30, 0x6A, 0x75, 0x73, 0x74, 0x2D, 0x73, 0x74, 0x6F, 0x70, 0x2D, 0x69, 0x74 ]]))

    # print(strToHex('just-stop-it'))
    # print(' '.join(['{:02X}'.format(c) for c in b'Ufw\x88']))